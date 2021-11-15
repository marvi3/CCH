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
class QueryServlet : public MapServer::Servlet {

public:

	QueryServlet(std::string handler, Algorithm& algorithm, const std::vector<Geometry::Point>& coordinates, const COORDINATE_TREE_TYPE& cs) :
		Servlet(handler),
		coordinates(coordinates),
		cs(cs),
		algorithm(algorithm) {
	}

	virtual ~QueryServlet() {}

    virtual void Process(mg_connection* conn, const mg_request_info* requestInfo) {
        MapServer::QueryStringParser qsp(requestInfo);
        readParameters(qsp);
        int sourceNode = getSourceNodeId(qsp);
        int targetNode = getTargetNodeId(qsp);
        std::cout << "   Source = " << String::prettyInt(sourceNode) << std::endl;
        std::cout << "   Target = " << String::prettyInt(targetNode)  << std::endl;
        if (targetNode == -1) {
            sendResponse(conn, algorithm.runSourceOnly(sourceNode));
        } else if (sourceNode == -1) {
            sendResponse(conn, algorithm.runTargetOnly(targetNode));
        } else {
            sendResponse(conn, algorithm.run(sourceNode, targetNode));
        }
        std::cout << "The calculation has been completed." << std::endl << std::endl << std::flush;
    }

	inline const std::map<std::string, Parameter>& getParameters() const {
		return algorithm.getParameters();
	}

protected:
	int getSourceNodeId(MapServer::QueryStringParser& qsp) {
		double lat = qsp.getValue<double>("SourceLat", 1000.0);
		double lon = qsp.getValue<double>("SourceLon", 1000.0);
		return getNodeId(Geometry::Point::LatLong(lat, lon));
	}

	int getTargetNodeId(MapServer::QueryStringParser& qsp) {
		double lat = qsp.getValue<double>("TargetLat", 1000.0);
		double lon = qsp.getValue<double>("TargetLon", 1000.0);
		return getNodeId(Geometry::Point::LatLong(lat, lon));
	}

	virtual int getNodeId(Geometry::Point pos) {
		if (pos.latitude == 1000.0 || pos.longitude == 1000.0) return -1;
		return cs.getNearestNeighbour(pos);
	}

	virtual void readParameters(MapServer::QueryStringParser& qsp) {
		for (std::map<std::string, Parameter>::iterator iterator = algorithm.getParameters().begin(); iterator != algorithm.getParameters().end(); iterator++) {
			iterator->second.value = qsp.getValue<string>(iterator->second.name, iterator->second.defaultValue);
			std::cout << "   Parameter(" << iterator->second.name << ") = " << iterator->second.value << std::endl;
		}
	}

	virtual void sendResponse(mg_connection* conn, vector<Result>&& response) {
		if (!response.empty()) {
			for (std::map<std::string, Parameter>::iterator iterator = algorithm.getParameters().begin(); iterator != algorithm.getParameters().end(); iterator++) {
				response.front().parameter.push_back(std::make_pair(iterator->second.name, iterator->second.value));
			}
		}
		MapServer::ResponseJSON jsonresponse(resultsToJSON(response, coordinates));
		jsonresponse.Write(conn);
	}

private:
	const std::vector<Geometry::Point>& coordinates;
	const COORDINATE_TREE_TYPE& cs;
	Algorithm& algorithm;

};
