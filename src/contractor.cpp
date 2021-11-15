/*
 * contractor.cpp
 *
 *  Created on: 03.01.2019
 *      Author: Agent M
 */

#include "contractor.h"

namespace std {

CCHGraph buildBackwardGraph(CCHGraph temp_forward_graph) {
	CCHGraph temp_backward_graph;
	vector<vector<unsigned>> backward_graph_head(temp_forward_graph.number_nodes);
	temp_backward_graph.first_out.push_back(0);

	unsigned current_node = 0;
	unsigned next_node = temp_forward_graph.first_out[1];

	for (unsigned i = 0; i < temp_forward_graph.number_edges; i++) {
		for (; i < next_node; i++) {
			unsigned temp_head = temp_forward_graph.head[i];
			backward_graph_head[temp_head].push_back(current_node);
		}
		next_node = temp_forward_graph.first_out[current_node];
		current_node++;
	}
	for (unsigned i = 1; i < temp_forward_graph.number_nodes; i++) {
		temp_backward_graph.first_out.push_back(temp_backward_graph.first_out[i - 1] + backward_graph_head[i].size());
		for (unsigned j = 0; j < backward_graph_head[i].size(); j++) {
			temp_backward_graph.head.push_back(backward_graph_head[i][j]);
		}
	}
	temp_backward_graph.number_nodes = temp_backward_graph.first_out.size() - 1;
	temp_backward_graph.number_edges = temp_backward_graph.head.size();
	return temp_backward_graph;
}

CCHGraph mergeGraph(CCHGraph temp_forward_graph, CCHGraph temp_backward_graph) {
	CCHGraph temp_merged_graph;
	vector<vector<unsigned>> graphHead(temp_forward_graph.number_nodes);
	temp_merged_graph.first_out.push_back(0);
	for(unsigned i = 0; i < temp_forward_graph.number_nodes; i++) {
		unsigned f = temp_forward_graph.first_out[i];
		unsigned b = temp_backward_graph.first_out[i];
		unsigned numberNodes = 0;
		while(f < temp_forward_graph.first_out[i + 1] || b < temp_backward_graph.first_out[i + 1]) {
			if(temp_forward_graph.head[f] <= temp_backward_graph.head[b] || b >= temp_backward_graph.first_out[i + 1]) {
				temp_merged_graph.head.push_back(temp_forward_graph.head[f]);
				f++;
				numberNodes ++;
			} else if(temp_backward_graph.head[b] <= temp_forward_graph.head[f] || f >= temp_forward_graph.first_out[i + 1]) {
				temp_merged_graph.head.push_back(temp_backward_graph.head[b]);
				b++;
				numberNodes ++;
			} else if(temp_forward_graph.head[f] == temp_backward_graph.head[b]) {
				temp_merged_graph.head.push_back(temp_forward_graph.head[f]);
				b++;
				f++;
				numberNodes ++;
			}
		}
		temp_merged_graph.first_out.push_back(temp_merged_graph.first_out[i] + numberNodes);
	}
	temp_merged_graph.number_nodes = temp_merged_graph.first_out.size() - 1;
	temp_merged_graph.number_edges = temp_merged_graph.head.size();
	return temp_merged_graph;
}

CCHGraph deleteDirections(CCHGraph forward_graph) {
	CCHGraph backward_graph = buildBackwardGraph(forward_graph);
	return mergeGraph(forward_graph, backward_graph);
}

vector<unsigned> backwardOrder(vector<unsigned> temp_order) {
	vector<unsigned> temp_backward_order(temp_order.size());
	for(unsigned i = 0; i < temp_order.size(); i++) {
		temp_backward_order[temp_order[i]] = i;
	}

	return temp_backward_order;
}

CCHGraph contractNodes(CCHGraph temp_graph) {
	temp_graph.backward_order = backwardOrder(temp_graph.order);
	vector<vector<unsigned>> temp_shortcut(temp_graph.number_nodes);
	CCHGraph shortcut_graph;
	shortcut_graph.first_out.push_back(0);
	unsigned edgecount = 0;

	for(unsigned i = 0; i < temp_graph.number_nodes; i++) {
		unsigned temp_start = temp_graph.first_out[temp_graph.backward_order[i]];
		unsigned temp_end = temp_graph.first_out[temp_graph.backward_order[i] + 1];
		unsigned temp_size = temp_end - temp_start;

		vector<unsigned> temp_clique_nodes(temp_size);
		for(unsigned j = temp_start; j < temp_end; j++) {
			temp_clique_nodes[j - temp_start] = temp_graph.head[j];
		}
		for(unsigned k = 0; k < temp_size; k++) {
			for(unsigned l = 0; l < temp_size; l++) {
				bool edge_existing = false;
				for(unsigned m = temp_graph.first_out[temp_clique_nodes[k]]; m < temp_graph.first_out[temp_clique_nodes[k] + 1]; m++) {
					if(temp_graph.head[m] == l) {
						edge_existing = true;
						m = temp_graph.first_out[temp_clique_nodes[k] + 1];
					}
				}
				if(!edge_existing) {
					temp_shortcut[k].push_back(temp_clique_nodes[l]);
					edgecount++;
				}
			}
		}
	}
	for(unsigned i = 0; i < temp_graph.number_nodes; i++) {
		unsigned j;
		for(j = 0; j < temp_shortcut[i].size(); j++) {
			shortcut_graph.head.push_back(temp_shortcut[i][j]);
		}
		shortcut_graph.first_out.push_back(shortcut_graph.first_out[i] + j);
	}
	return mergeGraph(temp_graph, shortcut_graph);
}

void buildClique(CCHGraph temp_graph, unsigned order) {
	for(unsigned i = 0; i < temp_graph.number_nodes; i++) {

	}
}


contractor::contractor() {
	// TODO Auto-generated constructor stub

}

contractor::~contractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
