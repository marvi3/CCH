/*
* CCHGraph.cpp
*
*  Created on: 17.12.2018
*      Author: Agent M
*/

#include <map>

#include "CCHGraph.h"
#include "vector_io.h"
#include "id_queue.h"
#include "stdafx.h"

namespace std {

        void CCHGraph::initiallize(string temp_directory, string order_path) {

		first_out = load_vector<unsigned>(temp_directory + "first_out");
		head = load_vector<unsigned>(temp_directory + "head");

		latitude = load_vector<unsigned>(temp_directory + "latitude");
		longitude = load_vector<unsigned>(temp_directory + "longitude");

		number_nodes = first_out.size();
		number_edges = head.size();

                weight = load_vector<unsigned>(temp_directory + "geo_distance");

                if (order_path != "") {
                    order = load_vector<unsigned>(order_path);
                }
                else {
                    order = load_vector<unsigned>(temp_directory + "flowcutter_order");
                }

                //if (travel_time) {
                //        order = load_vector<unsigned>(temp_directory + "flowcutter_order");
                //}
                //else {
                //        order = load_vector<unsigned>(temp_directory + "flowcutter_order");
                //}
	}

//	void CCHGraph::initWeightPosition() {
//		weight_Position.resize(first_out.size() - 1);
//		for (unsigned i = 0; i < first_out.size() - 1; i++) {
//			for (unsigned j = first_out[i]; j < first_out[i + 1]; j++) {
//				unsigned target_node = head[j];
//				weight_Position[i][target_node] = j;
//			}
//		}
//	}
	
	void CCHGraph::initReverseHead() {
		reverse_head.clear();
		reverse_head.resize(head.size());
		for (unsigned x = 0; x < first_out.size() - 1; x++) {
			for (unsigned j = first_out[x]; j < first_out[x + 1]; j++) {
				unsigned y = head[j];
				for (unsigned k = first_out[y]; k < first_out[y + 1]; k++) {
					if (head[k] == x) {
						reverse_head[j] = k;
					}
				}
			}
		}
	}

	unsigned CCHGraph::getWeightUp(unsigned x, unsigned y) {
		return weight_up[weight_Position[x][y]];
	}

	unsigned CCHGraph::getWeightDown(unsigned x, unsigned y) {
		return weight_down[weight_Position[x][y]];
	}

	unsigned CCHGraph::getWeight(unsigned x, unsigned y, bool direction) {
		unsigned n;
		if (direction == 0) {  //up
		    for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
		        n = head[out_arc];
		        //cout << "  u " << x << " -> " << n << "\n";
		        if (n == y) {
		            return weight_up[out_arc];
		        }
		    }
		}
		else {  //down
		    for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
		        n = head[out_arc];
		        //cout << "  d " << x << " -> " << n << "\n";
		        if (n == y) {
		            return weight_down[out_arc];
		        }
		    }
		}
		cout << "can't find weight! " << x << " -> " << y << "\n";
		return inf_weight;
	}

	unsigned CCHGraph::getWeight(unsigned x, unsigned y) {
	    for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
	        unsigned n = head[out_arc];
	        if (n == y) {
	            return weight[out_arc];
	        }
	    }
	    cout << "can't find weight! " << x << " -> " << y << "\n";
	    return inf_weight;
	}
	
//	void CCHGraph::setWeightUp(unsigned x, unsigned y, unsigned value) {
//		weight_up[weight_Position[x][y]] = value;
//	}
//		
//	void CCHGraph::setWeightDown(unsigned x, unsigned y, unsigned value) {
//		weight_down[weight_Position[x][y]] = value;
//	}
	
	void CCHGraph::setWeight(unsigned x, unsigned y, unsigned value, bool direction) {
	    unsigned n;
	    if (direction == 0) {  //up
	        for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
	            n = head[out_arc];
	            if (n == y) {
	                weight_up[out_arc] = value;
	                return;
	            }
	        }
	    }
	    else {  //down
	        for (unsigned out_arc = first_out[x]; out_arc < first_out[x+1]; ++out_arc) {
	            n = head[out_arc];
	            if (n == y) {
	                weight_down[out_arc] = value;
	                return;
	            }
	        }
	    }
	    cout << "can't find edge to set weight\n";
	}

        CCHGraph::CCHGraph(string directory, string order_name, bool temp_travel) {
		//constructor for existing graphs
		travel_time = temp_travel;
                initiallize(directory, order_name);
	}

	CCHGraph::CCHGraph(bool temp_travel) {
		//constructor for empty graphs
		travel_time = temp_travel;

	}

	CCHGraph::CCHGraph() {
		//standard constructor
	}

	CCHGraph::~CCHGraph() {
		//destructor
	}



} /* namespace std */
