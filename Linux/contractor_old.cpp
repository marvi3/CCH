/*
 * contractor.cpp
 *
 *  Created on: 03.01.2019
 *      Author: Agent M
 */

#include "contractor.h"
#include "CCHGraph.h"

namespace std {

CCHGraph contractor::buildBackwardGraph(CCHGraph temp_forward_graph) {
	CCHGraph temp_backward_graph;
	vector<vector<unsigned>> backward_graph_head(temp_forward_graph.first_out.size() - 1);
	temp_backward_graph.first_out.push_back(0);

    unsigned current_node = 0;
	if ((temp_forward_graph.first_out.size() - 1) > 1) {
		unsigned next_node;
		for (unsigned i = 0; i < temp_forward_graph.head.size();) {
			next_node = temp_forward_graph.first_out[current_node + 1];
			for (; i < next_node && i < temp_forward_graph.head.size(); i++) {
				unsigned temp_head = temp_forward_graph.head[i];
				backward_graph_head[temp_head].push_back(current_node);
			}
			current_node++;
		}
		for (unsigned i = 0; i < temp_forward_graph.first_out.size() - 1; i++) {
			temp_backward_graph.first_out.push_back(temp_backward_graph.first_out[i] + backward_graph_head[i].size());
			for (unsigned j = 0; j < backward_graph_head[i].size(); j++) {
				temp_backward_graph.head.push_back(backward_graph_head[i][j]);
			}
		}
	}
	
	return temp_backward_graph;
}

CCHGraph contractor::mergeGraph(CCHGraph temp_forward_graph, CCHGraph temp_backward_graph) {
	CCHGraph temp_merged_graph;
	vector<vector<unsigned>> graphHead(temp_forward_graph.first_out.size() - 1);
	temp_merged_graph.first_out.push_back(0);
	unsigned whole = 0;
	for(unsigned i = 0; i < temp_forward_graph.first_out.size() - 1; i++) {
		unsigned f = temp_forward_graph.first_out[i];
		unsigned b = temp_backward_graph.first_out[i];
		unsigned numberNodes = 0;
		while(f < temp_forward_graph.first_out[i + 1] || b < temp_backward_graph.first_out[i + 1]) {
			if (temp_forward_graph.head[f] == temp_backward_graph.head[b]) {
				temp_merged_graph.head.push_back(temp_forward_graph.head[f]);
				b++;
				f++;
				whole++;
				numberNodes++;
			} else if((temp_forward_graph.head[f] <= temp_backward_graph.head[b] || b >= temp_backward_graph.first_out[i + 1] - 1) && f < (temp_forward_graph.first_out[i + 1])) {
				temp_merged_graph.head.push_back(temp_forward_graph.head[f]);
				f++;
				whole++;
				numberNodes ++;
			} else if((temp_backward_graph.head[b] <= temp_forward_graph.head[f] || f >= temp_forward_graph.first_out[i + 1] - 1) && b < (temp_backward_graph.first_out[i + 1])) {
				temp_merged_graph.head.push_back(temp_backward_graph.head[b]);
				b++;
				whole++;
				numberNodes ++;
			}
		}
		temp_merged_graph.first_out.push_back(temp_merged_graph.first_out[i] + numberNodes);
	}
	temp_merged_graph.order = temp_forward_graph.order;
	temp_merged_graph.latitude = temp_forward_graph.latitude;
	temp_merged_graph.longitude = temp_forward_graph.longitude;
	temp_merged_graph.travel_time = temp_forward_graph.travel_time;
	return temp_merged_graph;
}

CCHGraph contractor::deleteDirections(CCHGraph forward_graph) {
	CCHGraph backward_graph = buildBackwardGraph(forward_graph);
	return mergeGraph(forward_graph, backward_graph);
}

vector<unsigned> contractor::backwardOrder(vector<unsigned> temp_order) {
	vector<unsigned> temp_backward_order(temp_order.size());
	for(unsigned i = 0; i < temp_order.size(); i++) {
		temp_backward_order[temp_order[i]] = i;
	}

	return temp_backward_order;
}

CCHGraph contractor::contractNodes(CCHGraph temp_graph) {
	temp_graph.backward_order = backwardOrder(temp_graph.order);
	vector<vector<unsigned>> temp_shortcut(temp_graph.first_out.size() - 1);
	CCHGraph shortcut_graph;
	shortcut_graph.backward_order = temp_graph.backward_order;
	shortcut_graph.order = temp_graph.order;
	shortcut_graph.first_out.push_back(0);
	for (unsigned i = 0; i < temp_graph.first_out.size() - 1; i++) {
		for (unsigned j = temp_graph.first_out[i]; j < temp_graph.first_out[i + 1]; j++) {
			temp_shortcut[i].push_back(temp_graph.head[j]);
		}
	}
	for (unsigned i = 0; i < temp_graph.first_out.size() - 1; i++) {
		unsigned temp_start = temp_graph.first_out[temp_graph.order[i]];
		unsigned temp_end = temp_graph.first_out[temp_graph.order[i] + 1];
		unsigned temp_size = temp_end - temp_start;

		vector<unsigned> temp_clique_nodes;
		for (unsigned j = temp_start; j < temp_end; j++) {
			temp_clique_nodes.push_back(temp_graph.head[j]);
		}
		for (unsigned k = 0; k < temp_size; k++) {
			for (unsigned l = 0; l < temp_size; l++) {
				if (l == k) continue;
				if (temp_graph.backward_order[temp_clique_nodes[l]] < temp_graph.backward_order[i] ||
					temp_graph.backward_order[temp_clique_nodes[k]] < temp_graph.backward_order[i]) continue;

				bool edge_existing = false;
				for (unsigned m = 0; m < temp_shortcut[temp_clique_nodes[k]].size(); m++) {
					if (temp_shortcut[temp_clique_nodes[k]][m] == temp_clique_nodes[l]) {
						edge_existing = true;
						m = temp_shortcut[temp_clique_nodes[k]].size();
					}
				}
				if (!edge_existing) {
					temp_shortcut[temp_clique_nodes[k]].push_back(temp_clique_nodes[l]);
				}
			}
		}
	}
	cout << shortcut_graph.first_out.size() << "<\n";
	for (unsigned i = 0; i < temp_graph.first_out.size() - 1; i++) {
		unsigned j;
		for (j = 0; j < temp_shortcut[i].size(); j++) {
			shortcut_graph.head.push_back(temp_shortcut[i][j]);
		}
		shortcut_graph.first_out.push_back(shortcut_graph.first_out[i] + j);
	}
	return shortcut_graph;
}

//CCHGraph contractor::contractNodes(CCHGraph temp_graph) {
//	temp_graph.backward_order = backwardOrder(temp_graph.order);
//	vector<vector<unsigned>> temp_shortcut(temp_graph.number_nodes);
//	CCHGraph shortcut_graph;
//	shortcut_graph.first_out.push_back(0);
//	unsigned edgecount = 0;
//
//	for(unsigned i = 0; i < temp_graph.number_nodes; i++) {
//		unsigned temp_start = temp_graph.first_out[temp_graph.backward_order[i]];
//		unsigned temp_end = temp_graph.first_out[temp_graph.backward_order[i] + 1];
//		unsigned temp_size = temp_end - temp_start;
//
//		vector<unsigned> temp_clique_nodes;
//		for(unsigned j = temp_start; j < temp_end; j++) {
//			temp_clique_nodes.push_back(temp_graph.head[j]);
//		}
//		for(unsigned k = 0; k < temp_size; k++) {
//			for(unsigned l = 0; l < temp_size; l++) {
//				bool edge_existing = false;
//				for(unsigned m = temp_graph.first_out[temp_clique_nodes[k]]; m < temp_graph.first_out[temp_clique_nodes[k] + 1]; m++) {
//					if(temp_graph.head[m] == l) {
//						edge_existing = true;
//						m = temp_graph.first_out[temp_clique_nodes[k] + 1];
//					}
//				}
//				if(!edge_existing) {
//					temp_shortcut[temp_clique_nodes[k]].push_back(temp_clique_nodes[l]);
//					edgecount++;
//				}
//			}
//		}
//	}
//	for(unsigned i = 0; i < temp_graph.number_nodes; i++) {
//		unsigned j;
//		for(j = 0; j < temp_shortcut[i].size(); j++) {
//			shortcut_graph.head.push_back(temp_shortcut[i][j]);
//		}
//		shortcut_graph.first_out.push_back(shortcut_graph.first_out[i] + j);
//	}
//	return mergeGraph(temp_graph, shortcut_graph);
//}

contractor::contractor() {
	// TODO Auto-generated constructor stub

}

contractor::~contractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
