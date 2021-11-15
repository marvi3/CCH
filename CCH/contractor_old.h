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
    static CCHGraph buildBackwardGraph(CCHGraph temp_forward_graph);
	static CCHGraph mergeGraph(CCHGraph temp_forward_graph, CCHGraph temp_backward_graph);
	static CCHGraph deleteDirections(CCHGraph forward_graph);
	static vector<unsigned> backwardOrder(vector<unsigned> temp_order);
	static CCHGraph contractNodes(CCHGraph temp_graph);
	static void buildClique(CCHGraph temp_graph, unsigned order);

	virtual ~contractor();
};

} /* namespace std */

#endif /* CONTRACTOR_H_ */
