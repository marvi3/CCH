/*
 * CCHGraph.h
 *
 *  Created on: 17.12.2018
 *      Author: Agent M
 */

#ifndef CCHGRAPH_H_
#define CCHGRAPH_H_

#include "CCHGraph.h"
#include "vector_io.h"
#include "id_queue.h"

namespace std {

class CCHGraph {
public:
	CCHGraph(string directory, bool temp_travel);
	CCHGraph(bool temp_travel);
	virtual ~CCHGraph();

	vector<unsigned> first_out;
	vector<unsigned> head;

	vector<unsigned> order;
	vector<unsigned> backward_order;

	vector <unsigned> latitude;
	vector <unsigned> longitude;

	bool travel_time;

	unsigned number_nodes = first_out.size();
	unsigned number_edges = head.size();

private:
	void initiallize(string directory);
};

} /* namespace std */

#endif /* CCHGRAPH_H_ */
