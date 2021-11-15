#pragma once

#include <map>
#include <vector>

#include "../../DataStructures/Result.h"
#include "../../DataStructures/Parameter.h"

class Algorithm {

public:
	Algorithm() {}

	virtual ~Algorithm() {}

        virtual std::vector<Result> run(int sourceNodeId, int targetNodeId) = 0;

        virtual std::vector<Result> run2(int sourceNodeId, int targetNodeId, vector<unsigned> first_out, vector<unsigned> head) = 0;

	virtual std::vector<Result> runSourceOnly(int sourceNodeId) {
	    std::vector<Result> resultSet;
		resultSet.push_back(Result("Source Node", KIT::green));
		resultSet.back().nodes.push_back(Result::Node(sourceNodeId, "S"));
		return resultSet;
	}

	virtual std::vector<Result> runTargetOnly(int targetNodeId) {
	    std::vector<Result> resultSet;
		resultSet.push_back(Result("Target Node", KIT::green));
		resultSet.back().nodes.push_back(Result::Node(targetNodeId, "T"));
		return resultSet;
	}

    inline const std::map<std::string, Parameter>& getParameters() const {
        return parameters;
    }

	inline std::map<std::string, Parameter>& getParameters() {
		return parameters;
	}

protected:
	inline void addParameter(Parameter p) {
		parameters[p.name] = p;
	}

	inline void constParameter(const std::string& name, const std::string& value) {
	    std::map<std::string, Parameter>::iterator position = parameters.find(name);
		if (position != parameters.end()) {
			constParameters[position->first] = position->second;
			parameters.erase(position);
		}
	}

	template <typename T>
	inline T getParameter(const std::string& name) {
	    std::map<std::string, Parameter>::iterator position = parameters.find(name);
		if (position != parameters.end()) {
			return position->second.getValue<T>();
		} else {
			position = constParameters.find(name);
			if (position != constParameters.end()) {
				return position->second.getValue<T>();
			} else {
			    std::cout << "WARNING! Parameter " << name << " not found, returning: " << T() << std::endl;
				return T();
			}
		}
	}

    template <typename T>
    inline void setParameter(const std::string& name, const T& value) {
        std::map<std::string, Parameter>::iterator position = parameters.find(name);
        if (position != parameters.end()) {
            position->second.value = std::to_string(value);
        }
    }

    std::map<std::string, Parameter> parameters;

private:
    std::map<std::string, Parameter> constParameters;

};
