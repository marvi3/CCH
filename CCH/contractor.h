/*
 * contractor.h
 *
 *  Created on: 03.01.2019
 *      Author: Agent M
 */

#include "CCHGraph.h"

#ifndef CONTRACTOR_H_
#define CONTRACTOR_H_

namespace std {

class contractor {
public:
	contractor();
	static void buildUndirectedGraph(CCHGraph *temp_forward_graph);
	static vector<unsigned> backwardOrder(vector<unsigned> &temp_order);
	static void contractNodes(CCHGraph *temp_graph);

	virtual ~contractor();

private:
    static bool edgeExists(CCHGraph *g, unsigned x, unsigned y);
    static void contractNode(CCHGraph *g, unsigned x);
};

} /* namespace std */

#endif /* CONTRACTOR_H_ */
