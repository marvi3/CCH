#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Servlet.h"

#include "../QueryStringParser.h"
#include "../Responses/ResponseJSON.h"

#include "../../Helpers/Helpers.h"
#include "../../Helpers/JSONString.h"
#include "../../Helpers/Function.h"

#include "../../Algorithms/MapServer/Algorithm.h"

#include "../../DataStructures/Geometry/Point.h"
#include "../../DataStructures/Parameter.h"
#include "../../DataStructures/Result.h"

template<typename COORDINATE_TREE_TYPE>
class QueryServletWithVertexInput : public MapServer::Servlet {

public:

    QueryServletWithVertexInput(std::string handler, Algorithm& algorithm, const std::vector<Geometry::Point>& coordinates, const COORDINATE_TREE_TYPE& cs) :
        Servlet(handler),
        coordinates(coordinates),
        cs(cs),
        algorithm(algorithm),
        parameters(algorithm.getParameters()) {
        parameters["overwriteVertices"] = Parameter("bool", "overwriteVertices", "Use manual vertex IDs", "false", false, 1000);
        parameters["sourceId"] = Parameter("int", "sourceId", "Source Id", "-1", true, 1001);
        parameters["targetId"] = Parameter("int", "targetId", "Target Id", "-1", true, 1002);
    }

    virtual ~QueryServletWithVertexInput() {}

    virtual void Process(mg_connection* conn, const mg_request_info* requestInfo) {
        MapServer::QueryStringParser qsp(requestInfo);
        readParameters(qsp);
        int sourceNode = getSourceNodeId(qsp);
        int targetNode = getTargetNodeId(qsp);
        std::cout << "   Source = " << String::prettyInt(sourceNode) << std::endl;
        std::cout << "   Target = " << String::prettyInt(targetNode)  << std::endl;
        if (targetNode == -1) {
            sendResponse(conn, algorithm.runSourceOnly(sourceNode), sourceNode, targetNode);
        } else if (sourceNode == -1) {
            sendResponse(conn, algorithm.runTargetOnly(targetNode), sourceNode, targetNode);
        } else {
            sendResponse(conn, algorithm.run(sourceNode, targetNode), sourceNode, targetNode);
        }
        std::cout << "The calculation has been completed." << std::endl << std::endl << std::flush;
    }

    inline const std::map<std::string, Parameter>& getParameters() const {
        return parameters;
    }

protected:
    int getSourceNodeId(MapServer::QueryStringParser& qsp) {
        if (parameters.find("overwriteVertices")->second.template getValue<bool>()) {
            return parameters.find("sourceId")->second.template getValue<int>();
        } else {
            double lat = qsp.getValue<double>("SourceLat", 1000.0);
            double lon = qsp.getValue<double>("SourceLon", 1000.0);
            return getNodeId(Geometry::Point::LatLong(lat, lon));
        }
    }

    int getTargetNodeId(MapServer::QueryStringParser& qsp) {
        if (parameters.find("overwriteVertices")->second.template getValue<bool>()) {
            return parameters.find("targetId")->second.template getValue<int>();
        } else {
            double lat = qsp.getValue<double>("TargetLat", 1000.0);
            double lon = qsp.getValue<double>("TargetLon", 1000.0);
            return getNodeId(Geometry::Point::LatLong(lat, lon));
        }
    }

    virtual int getNodeId(Geometry::Point pos) {
        if (pos.latitude == 1000.0 || pos.longitude == 1000.0) return -1;
        return cs.getNearestNeighbour(pos);
    }

    virtual void readParameters(MapServer::QueryStringParser& qsp) {
        for (std::map<std::string, Parameter>::iterator iterator = algorithm.getParameters().begin(); iterator != algorithm.getParameters().end(); iterator++) {
            iterator->second.value = qsp.getValue<string>(iterator->second.name, iterator->second.defaultValue);
        }
        for (std::map<std::string, Parameter>::iterator iterator = parameters.begin(); iterator != parameters.end(); iterator++) {
            iterator->second.value = qsp.getValue<string>(iterator->second.name, iterator->second.defaultValue);
            std::cout << "   Parameter(" << iterator->second.name << ") = " << iterator->second.value << std::endl;
        }
    }

    virtual void sendResponse(mg_connection* conn, vector<Result>&& response, int sourceId = -1, int targetId = -1) {
        if (!response.empty()) {
            for (std::map<std::string, Parameter>::iterator iterator = algorithm.getParameters().begin(); iterator != algorithm.getParameters().end(); iterator++) {
                response.front().parameter.push_back(std::make_pair(iterator->second.name, iterator->second.value));
            }
            response.front().parameter.push_back(std::make_pair("sourceId", std::to_string(sourceId)));
            response.front().parameter.push_back(std::make_pair("targetId", std::to_string(targetId)));
        }
        MapServer::ResponseJSON jsonresponse(resultsToJSON(response, coordinates));
        jsonresponse.Write(conn);
    }

private:
    const std::vector<Geometry::Point>& coordinates;
    const COORDINATE_TREE_TYPE& cs;
    Algorithm& algorithm;

    std::map<std::string, Parameter> parameters;

};
