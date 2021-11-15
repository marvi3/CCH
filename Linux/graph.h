#include <vector>

#ifndef GRAPH_H_
#define GRAPH_H_

using namespace std;

struct graph1 {
	vector<unsigned> first_out;
	vector<unsigned> head;
	vector<unsigned> order;
	vector<unsigned> backward_order;
	
	//after perfect customization we need to manage up and down graphs separately
	vector<unsigned> first_out_up;
	vector<unsigned> head_up;
	vector<unsigned> weight_up;

	vector<unsigned> first_out_down;
	vector<unsigned> head_down;
	vector<unsigned> weight_down;

	unsigned number_nodes = first_out.size();
	unsigned number_edges = head.size();
};

#endif /* GRAPH_H_ */
