#include "constants.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

#include "CCHGraph.h"
#include "contractor.h"
#include "Customizer.h"
#include "graph.h"
#include "QueryManager.h"
#include "stdafx.h"

using namespace std;

string directory = "../stupferich/";
string order_path;

bool perfect_customization = true;

bool complete_init_triangles = false;

//If the travel-time should be the measure then set this to true. When set to false the geo_distance will be used.
bool travel_time = true;

unsigned num_queries = 1000;

CCHGraph test_graph;

CCHGraph original_graph, undirected_graph;

Customizer customizer;

vector<unsigned> weight;

void preProcessing();
void printGraph(CCHGraph *g, QueryManager *q);
void init_weight(CCHGraph *g);
unsigned find_weight(unsigned, unsigned, CCHGraph*, vector<map<unsigned, unsigned>>&);
unsigned dijkstra(CCHGraph *g, unsigned s, unsigned t);
unsigned dijkstra_primitive(CCHGraph *g, unsigned s, unsigned t);



//Set the string of the directory where the graph is located. Use \\ instead of \. Write a \\ at the end of the string.
//first_out, head, latitude and longitude should be directly in the location that is specified,
//while order and weight should be in a geo_distance_ch and travel_time_ch subfolder


auto ttw = load_vector<unsigned>(directory + "travel_time");
vector<map<unsigned, unsigned>> tt_weight;


unsigned contraction_time = 0;






void init_weight(CCHGraph *g) {
    unsigned y = 0;
    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        map<unsigned, unsigned> tmap;
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            //cout << ". " << out_arc << "  "<< ttw.size()<< "\n";
            tmap[y] = ttw[out_arc];
            //cout << x << " " << y << " " << g->weight[out_arc] << "\n";
            //tmap[y] = g->weight[out_arc];
        }
        tt_weight.push_back(tmap);
    }
}

unsigned find_weight(unsigned x, unsigned y, CCHGraph *g, vector<map<unsigned, unsigned>> &w) {
    /*unsigned n = 0;
    auto it = w[x].find(y);

    if (it != w[x].end()) return it->second;

    return inf_weight;*/
    return g->getWeight(x,y);
}



int main(int argc, char* argv[]) {
    
    auto t_co_0 = std::chrono::high_resolution_clock::now();
    string query_type = "elimination_tree";


    if (argc >= 2) {
        string file_path(argv[1]);
        directory = file_path;
        cout << "Path: " << file_path << "\n";
    }
    if (argc >= 3) {
        string temp_o(argv[2]);
        order_path = temp_o;
        cout << "Order: " << temp_o << "\n";
    }

    auto test_travel_time = load_vector<unsigned>(directory + "test/travel_time_length");
    if (argc >= 4) {
        string temp_w(argv[3]);
        cout << "Metric: " << temp_w << "\n";
        test_travel_time = load_vector<unsigned>(directory + "test/" + temp_w + "_length");
        ttw = load_vector<unsigned>(directory + temp_w);
    }
    if (argc >= 5) {
        string temp_qt(argv[4]);
        query_type = temp_qt;
        cout << "Query Type: " << query_type << "\n";
    }
	if (argc >= 6) {
		string temp_at(argv[5]);
		if (temp_at == "all_triangles") {
			cout << "Complete Init Triangles: true\n";
			complete_init_triangles = true;
		}
	}

    auto test_source = load_vector<unsigned>(directory + "test/source");
    auto test_target = load_vector<unsigned>(directory + "test/target");


    preProcessing();

    auto t_co_1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_contraction = std::chrono::duration_cast<std::chrono::duration<double>>(t_co_1 - t_co_0);
    cout << "The contraction took " << time_contraction.count() << " seconds \n";
    cout << "the contracted graph has " << undirected_graph.head.size() << " edges \n";
    cout << "the uncontracted graph has " << original_graph.head.size() << " edges \n";


    init_weight(&original_graph);

    cout << "\n";


	auto t_cu_0 = std::chrono::high_resolution_clock::now();

    customizer.customize(&undirected_graph, tt_weight);

	auto t_cu_1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_customization = std::chrono::duration_cast<std::chrono::duration<double>>(t_cu_1 - t_cu_0);
	cout << "The customization took " << time_customization.count() << " seconds \n";

    cout << "\n";

    QueryManager test_manager (undirected_graph);

    cout << "running tests:\n";
    unsigned error_count = 0;
    unsigned summed_error = 0;
    unsigned solution = 0;

    auto t_qu_0 = std::chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < num_queries; ++i) {
        if (query_type == "elimination_tree") {
            solution = test_manager.eliminationTreeQuery(test_source[i], test_target[i]);
        }
        else if (query_type == "basic"){
            solution = test_manager.basicQuery(test_source[i], test_target[i]);
        }
        //cout << i << " " << test_source[i] << " - " << test_target[i] << "\n";

        if (solution != test_travel_time[i]) {
            //cout << i << "  " << test_source[i] << " -> " << test_target[i] << " = " << test_travel_time[i] << " - " << solution
            //     << " dijk: " << dijkstra(&undirected_graph, test_source[i], test_target[i])
            //     << "\n\n\n";

			cout << "Query " << i << " produced a wrong result. The expected length was " << test_travel_time[i]
				<< " but the actual result was " << solution << "\n";
            error_count++;
            if (solution < inf_weight) {
                summed_error += solution - test_travel_time[i];
            }
        }

        //auto t_up_0 = std::chrono::high_resolution_clock::now();
        //vector<unsigned> path = test_manager.unpack_path();
        //auto t_up_1 = std::chrono::high_resolution_clock::now();

        /*cout << test_source[i] << "->" << test_target[i] <<"\n";
        for (unsigned i = 0; i < path.size(); ++i) {
            cout << path[i] << " - ";
        }
        cout << "\n";*/
	    

    }

    cout << "errors: " << error_count << " out of " << num_queries << "\n";

	auto t_qu_1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_query = std::chrono::duration_cast<std::chrono::duration<double>>(t_qu_1 - t_qu_0);
	cout << num_queries <<" queries took " << time_query.count() << " seconds.\n"
		<< "This is " << time_query.count() / num_queries * 1000 << " miliseconds per query.\n" ;

	unsigned sum_of_nodes = 0;
    unsigned sum_of_edges = 0;

	for (unsigned i = 0; i < test_manager.visited_nodes.size(); i++) {
            sum_of_nodes += test_manager.visited_nodes[i];
	}
    for (unsigned i = 0; i < test_manager.visited_edges.size(); i++) {
        sum_of_edges += test_manager.visited_edges[i];
    }

    cout << "Every query visited " << sum_of_nodes / num_queries << " nodes on average.\n";
    cout << "   " << sum_of_edges / num_queries << " edges on average.\n";

    cin.peek();
    cout << "--------------------------------------\n\n\n";
    return 0;
}


void printGraph(CCHGraph *g, QueryManager *q) {
    unsigned y;
    cout << "size: " << g->first_out.size() << "\n";
    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];

            cout << x << " (" << find_weight(x, y, g, tt_weight) << ")->" << g->head[out_arc] << "\n";
        }
        cout << "\n";
    }
}


void preProcessing() {
    CCHGraph directed_graph(directory, order_path, travel_time);
    original_graph = directed_graph;
    undirected_graph = directed_graph;

    contractor::contractNodes(&undirected_graph);

//	undirected_graph.initWeightPosition();

	if (complete_init_triangles) {
		customizer.initTrianglesCompletely(&undirected_graph);
	}
	else {
		customizer.initTriangles(&undirected_graph);
	}

    undirected_graph.initReverseHead();
	undirected_graph.perf_cust = perfect_customization;
}

unsigned dijkstra(CCHGraph *g, unsigned s, unsigned t) {
    auto vnum = g->first_out_up.size();
    vector<unsigned> d;
    auto q = new MinIDQueue(vnum);

    vector<unsigned> predecessor(g->first_out_up.size());

    for (int i = 0; i < vnum; i++) {
        d.push_back(inf_weight);
    }
    cout << "s: "<< s << "\n";
    d[s] = 0;
    IDKeyPair idkp;
    idkp.id = s;
    idkp.key = 0;
    q->clear();
    q->push(idkp);
    int x = 0;

    while (!q->empty()) {
        x = q->pop().id;
        for (unsigned out_arc = g->first_out_up[x]; out_arc < g->first_out_up[x+1]; ++out_arc) {
            unsigned y = g->head_up[out_arc];
            if (d[x] + g->weight_up[out_arc] < d[y]) {
                d[y] = d[x] + g->weight_up[out_arc];
                predecessor[y] = x;
                if (q->contains_id(y)) {
                    idkp.id = y;
                    idkp.key = d[y];
                    q->decrease_key(idkp);
                }
                else {
                    idkp.id = y;
                    idkp.key = d[y];
                    q->push(idkp);
                }
            }
        }
    }

    x = t;
    vector<unsigned> temp_path;
    while (true) {
        temp_path.push_back(x);
        x = predecessor[x];
        if (x == s) break;
    }
    temp_path.push_back(s);

    cout << "\npath(" << s << ", " << t << "): ";
    for (int i = temp_path.size()-1; i > 0; --i) {
        cout << temp_path[i] << "(" << g->getWeight(temp_path[i], temp_path[i-1], 0) << ") - ";
    }
    cout << t << "\n";

    return d[t];
}


unsigned dijkstra_primitive(CCHGraph *g, unsigned s, unsigned t) {
    auto vnum = g->first_out.size();
    vector<unsigned> d;
    auto q = new MinIDQueue(vnum);

    vector<unsigned> predecessor(g->first_out.size());

    for (int i = 0; i < vnum; i++) {
        d.push_back(inf_weight);
    }
    cout << "s: "<< s << "\n";
    d[s] = 0;
    IDKeyPair idkp;
    idkp.id = s;
    idkp.key = 0;
    q->clear();
    q->push(idkp);
    int x = 0;

    while (!q->empty()) {
        x = q->pop().id;
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x+1]; ++out_arc) {
            unsigned y = g->head[out_arc];
            if (d[x] + g->weight[out_arc] < d[y]) {
                d[y] = d[x] + g->weight[out_arc];
                predecessor[y] = x;
                if (q->contains_id(y)) {
                    idkp.id = y;
                    idkp.key = d[y];
                    q->decrease_key(idkp);
                }
                else {
                    idkp.id = y;
                    idkp.key = d[y];
                    q->push(idkp);
                }
            }
        }
    }

    x = t;
    vector<unsigned> temp_path;
    while (true) {
        temp_path.push_back(x);
        x = predecessor[x];
        if (x == s) break;
    }
    temp_path.push_back(s);

    cout << "\npath(" << s << ", " << t << "): ";
    for (int i = temp_path.size()-1; i > 0; --i) {
        cout << temp_path[i] << "(" << g->getWeight(temp_path[i], temp_path[i-1]) << ") - ";
    }
    cout << t << "\n";

    return d[t];
}

