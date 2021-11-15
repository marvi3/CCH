#pragma once

#include <string>

#include "Response.h"

namespace MapServer {

class ResponseJSON : public Response {

public:
	ResponseJSON(const string contents) : Response("application/json", contents) {}
	virtual ~ResponseJSON() {}

};

}
