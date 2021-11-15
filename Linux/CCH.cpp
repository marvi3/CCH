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


CCHGraph test_graph;

CCHGraph original_graph, undirected_graph;

vector<unsigned> weight;

void init();
void contraction();
void printGraph(CCHGraph *g, QueryManager *q);
void init_weight(CCHGraph *g);
unsigned find_weight(unsigned, unsigned, CCHGraph*, vector<map<unsigned, unsigned>>&);
void run_test();
unsigned dijkstra(CCHGraph *g, unsigned s, unsigned t);
unsigned dijkstra_primitive(CCHGraph *g, unsigned s, unsigned t);



//Set the string of the directory where the graph is located. Use \\ instead of \. Write a \\ at the end of the string.
//first_out, head, latitude and longitude should be directly in the location that is specified,
//while order and weight should be in a geo_distance_ch and travel_time_ch subfolder
string directory_felix = "C:\\Users\\Agent M\\Desktop\\Dateien Felix\\Dokumente\\ernstes Zeug\\Uni\\Praktikum\\Algorithmen f�r Routenplanung\\cch\\stupferich\\";
string directory_dimitri = "../germany/";
//string directory_dimitri = "/amd.home/algoDaten/praktikum/graph/stupferich/";

// if this value is true dimitris direction is chosen, otherwise Felix direction is used
bool dir_dim = true;

//If the travel-time should be the measure then set this to true. When set to false the geo_distance will be used.
bool travel_time = true;

auto ttw = load_vector<unsigned>(directory_dimitri + "travel_time");
vector<map<unsigned, unsigned>> tt_weight;


CCHGraph test_graph2(directory_dimitri, travel_time);
unsigned contraction_time = 0;

auto test_source = load_vector<unsigned>(directory_dimitri + "test/source");
auto test_target = load_vector<unsigned>(directory_dimitri + "test/target");
auto test_travel_time = load_vector<unsigned>(directory_dimitri + "test/travel_time_length");




void init_weight(CCHGraph *g) {

    /*for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        cout << "iwg " << x << ": ";
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x+1]; ++out_arc) {
            unsigned y = g->head[out_arc];
            cout << " " << y << "(" << g->weight[out_arc] << ")";
        }
        cout << "\n";
    }*/

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

    contraction();

    /*for (unsigned x = 0; x < undirected_graph.first_out.size()-1; ++x) {
        cout << x << ": ";
        for (unsigned out = undirected_graph.first_out[x]; out < undirected_graph.first_out[x+1]; ++out) {
            unsigned y = undirected_graph.head[out];
            cout << y << " - ";
        }
        cout << "\n";
    }*/

    //return 0;

    init_weight(&original_graph);

    cout << "-1\n";

    Customizer customizer;
    customizer.customize(&undirected_graph, tt_weight);

    cout << "-2\n";

    QueryManager test_manager (undirected_graph);

    //save_vector("C:\\Users\\Hoehler\\Documents\\uni\\cch\\cch-b31516c\\saved_graphs\\karlsruhe\\first_out", undirected_graph.first_out);
    //save_vector("C:\\Users\\Hoehler\\Documents\\uni\\cch\\cch-b31516c\\saved_graphs\\karlsruhe\\head", undirected_graph.head);
    //auto path = test_manager.unpack_path();

    /*unsigned ts = strtoul(argv[1], nullptr, 10);
    unsigned tt = strtoul(argv[2], nullptr, 10);

    cout << "s: " << ts << " - t: " << tt << "\n";

    test_manager.eliminationTreeQuery(ts, tt);
    save_vector("../../../saved_graphs/cch_response", test_manager.unpack_path());*/

    //unsigned s1 = test_source[450];//486, 454, 453, 451, 450
    //unsigned t1 = test_target[450];

    //cout << "cust_dist: " << test_manager.eliminationTreeQuery(46, 23) << "\n";
    //auto temp_path = test_manager.unpack_path();

    /*cout << "\ncust_path(" << s1 << ", " << t1 << "): ";
    for (int i = 0; i < temp_path.size()-1; ++i) {
        cout << temp_path[i] << "(" << undirected_graph.getWeight(temp_path[i], temp_path[i+1], 0) << ") - ";
    }
    cout << t1 << "\n";*/

    //cout << dijkstra_primitive(&original_graph, s1, t1) << "\n";
    //cout << dijkstra_primitive(&original_graph, 46, 23) << "\n";
    //return 0;

    cout << "running tests:\n";
    unsigned error_count = 0;
    unsigned test_num = 10000;
    unsigned summed_error = 0;


    for (unsigned i = 0; i < test_num; ++i) {
        unsigned solution = test_manager.eliminationTreeQuery(test_source[i], test_target[i]);
        if (solution != test_travel_time[i]) {
            //cout << i << "  " << test_source[i] << " -> " << test_target[i] << " = " << test_travel_time[i] << " - " << solution
            //     << " dijk: " << dijkstra(&undirected_graph, test_source[i], test_target[i])
            //     << "\n\n\n";
            ////test_manager.unpack_path();
            error_count++;
            if (solution < inf_weight) {
                summed_error += solution - test_travel_time[i];
            }
        }
    }

    cout << "errors: " << error_count << " out of " << test_num << "\n";

    //cin.peek();
    cout << "end\n";
    return 0;
}


void run_test() {

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


void init() {

}

void contraction() {
        string directory_temp;
        if (dir_dim) {
                directory_temp = directory_dimitri;
        }
        else {
                directory_temp = directory_felix;
        }
    CCHGraph directed_graph(directory_temp, travel_time);
    original_graph = directed_graph;
    undirected_graph = directed_graph;

    contractor::contractNodes(&undirected_graph);
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

