/*
* CCHGraph.cpp
*
*  Created on: 17.12.2018
*      Author: Agent M
*/

#include "CCHGraph.h"
#include "vector_io.h"
#include "id_queue.h"
#include "stdafx.h"

namespace std {

	void CCHGraph::initiallize(string temp_directory) {

                first_out = load_vector<unsigned>(temp_directory + "first_out");
                head = load_vector<unsigned>(temp_directory + "head");

		latitude = load_vector<unsigned>(temp_directory + "latitude");
		longitude = load_vector<unsigned>(temp_directory + "longitude");

		number_nodes = first_out.size();
		number_edges = head.size();

                weight = load_vector<unsigned>(temp_directory + "travel_time");

		if (travel_time) {
                        order = load_vector<unsigned>(temp_directory + "flowcutter_order");
		}
		else {
                        order = load_vector<unsigned>(temp_directory + "flowcutter_order");
		}
	}

	CCHGraph::CCHGraph(string directory, bool temp_travel) {
		//constructor for existing graphs
		travel_time = temp_travel;
		initiallize(directory);
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

} /* namespace std */
