#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Algorithm.h"

#include "../../DataStructures/Parameter.h"
#include "../../DataStructures/Result.h"

// Example implementation of an "Algorithm".
// The example shows how to read parameter values from the browser and how to construct a Result object.
class HelloWorld : public Algorithm {

public:
    HelloWorld() : Algorithm() {
        // Adding four exemplary parameters for this algorithm.
        // Each parameter is specified using at least five values:
        // 1. First the type (written as string) which is used to select a suitable input form in the browser.
        // 2. The parameter name (also as string) which is used by HTTP GET to transmit the parameter.
        // 3. A String displayed to the user.
        // 4. The default value of the parameter.
        // 5. A bool that specifies if the algorithm should be reevaluated upon parameter changes.
        addParameter(Parameter("int", "anInt", "some integer value", "42", true));
        addParameter(Parameter("bool", "aBool", "a checkbox", "true", true));
        addParameter(Parameter("double", "aDouble", "some double value", "3.1415926", true));
        addParameter(Parameter("time", "aTime", "some time", "3723", true));
    }

    // This method is executed every time this algorithm is requested by a user from the browser.
    // You can also overwrite versions of this requiring only a source Node or a target Node.
    // sourceNodeId (respectively targetNodeId) is the index from your coordinates vector that is closest to the point clicked by the user.
    virtual vector<Result> run(int sourceNodeId, int targetNodeId) {
        // Use getParameter<>() to get the value of a parameter.
        int anInt = getParameter<int>("anInt");
        bool aBool = getParameter<bool>("aBool");
        double aDouble = getParameter<double>("aDouble");
        int aTime = getParameter<int>("aTime");
        std::cout << "You are running the Hello World Algorithm!" << std::endl
                  << "Your input parameters are:" << std::endl
                  << "   anInt   = " << anInt << std::endl
                  << "   aBool   = " << aBool << std::endl
                  << "   aDouble = " << aDouble << std::endl
                  << "   aTime   = " << aTime << " (" << String::secToString(aTime) << ")" << std::endl;
        // A path (displayed as polyline) is a vector of coordinate indices.
        vector<int> path({sourceNodeId, targetNodeId});
        // Construction of one result object. Your algorithm may return multiple of these results in a vector.
        // The string used as constructor parameter is displayed to the user. You may use HTML within this string.
        Result result("Hello World");
        // The color of the result
        result.color = KIT::green;
        // Single nodes/vertices that should be displayed on the map.
        result.nodes.push_back(Result::Node(sourceNodeId, "Source", KIT::blue));
        result.nodes.push_back(Result::Node(targetNodeId, "Target", KIT::red));
        // A display a path on the map
        result.pathes.push_back(Result::Path(path, "A Path", KIT::orange));
        // You may want to change (or correct) a parameter value:
        setParameter("aTime", 7302);
        return vector<Result>({result});
    }

    virtual vector<Result> run2(int sourceNodeId, int targetNodeId, vector<unsigned> first_out, vector<unsigned> head) {
        // Use getParameter<>() to get the value of a parameter.
        int anInt = getParameter<int>("anInt");
        bool aBool = getParameter<bool>("aBool");
        double aDouble = getParameter<double>("aDouble");
        int aTime = getParameter<int>("aTime");
        std::cout << "You are running the Hello World Algorithm!" << std::endl
                  << "Your input parameters are:" << std::endl
                  << "   anInt   = " << anInt << std::endl
                  << "   aBool   = " << aBool << std::endl
                  << "   aDouble = " << aDouble << std::endl
                  << "   aTime   = " << aTime << " (" << String::secToString(aTime) << ")" << std::endl;
        // A path (displayed as polyline) is a vector of coordinate indices.
        vector<int> path({sourceNodeId, targetNodeId});

        Result result("Hello World");

        int y = 0;

        for (unsigned x = 0; x < first_out.size()-1; ++x) {
            for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
                y = head[out_arc];
                vector<int> p({(int)x, (int)y});

                result.pathes.push_back(Result::Path( p, "", KIT::orange));
            }
        }


        // Construction of one result object. Your algorithm may return multiple of these results in a vector.
        // The string used as constructor parameter is displayed to the user. You may use HTML within this string.

        // The color of the result
        result.color = KIT::green;
        // Single nodes/vertices that should be displayed on the map.
        result.nodes.push_back(Result::Node(sourceNodeId, "Source", KIT::blue));
        result.nodes.push_back(Result::Node(targetNodeId, "Target", KIT::red));
        // A display a path on the map
        result.pathes.push_back(Result::Path(path, "A Path", KIT::green));
        // You may want to change (or correct) a parameter value:
        setParameter("aTime", 7302);
        return vector<Result>({result});
    }

};
