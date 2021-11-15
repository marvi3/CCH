/*
 * contractor.cpp
 *
 *  Created on: 03.01.2019
 *      Author: Agent M
 */

#include "contractor.h"
#include "CCHGraph.h"

#include <list>

#include <unordered_map>
#include <map>

namespace std {

CCHGraph *globalGraph;

struct less_than_rank {
    inline bool operator() (const unsigned &x, const unsigned &y) {
        return (globalGraph->rank[x] < globalGraph->rank[y]);
    }
};

void contractor::buildUndirectedGraph(CCHGraph *g) {
    vector<vector<unsigned>> edges(g->first_out.size());

    unsigned y = 0;
    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            //if (!edgeExists(g, x, y)) {
                edges[x].push_back(y);
            //}
            //if (!edgeExists(g, y, x)) {
                edges[y].push_back(x);
            //}
        }
    }

    unsigned counter = 0;
    vector<unsigned> temp_first_out;
    vector<unsigned> temp_head;

    temp_first_out.push_back(0);

    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        for (unsigned i = 0; i < edges[x].size(); ++i) {
            //cout << i << " - \n";
            counter++;
            temp_head.push_back(edges[x][i]);
        }
        temp_first_out.push_back(counter);
    }

    g->first_out = temp_first_out;
    g->head = temp_head;
}

bool contractor::edgeExists(CCHGraph *g, unsigned x, unsigned y) {
    for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
        if (y == g->head[out_arc]) return true;
    }
    return false;
}

void contractor::contractNodes(CCHGraph *g) {
    globalGraph = g;

    g->rank = backwardOrder(g->order);
    buildUndirectedGraph(g);
    //cout << "l1\n";
    vector<map<unsigned, bool>> edge_maps(g->first_out.size());

    vector<list<unsigned>> edges_list(g->first_out.size());
    
    unsigned y = 0;
    vector<vector<unsigned>> edges(g->first_out.size());

    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
            y = g->head[out_arc];
            edge_maps[i][y] = true;

            bool edge_found = false;
            for (unsigned i1 = 0; i1 < edges[i].size(); ++i1) {
                if (y == edges[i][i1] || y == i) edge_found = true;
            }
            if (!edge_found) {
                edges[i].push_back(y);
                edges_list[i].push_back(y);
            }
        }
    }

    unordered_map<unsigned, bool> node_map1;
    unordered_map<unsigned, bool> node_map2;
    vector<unsigned> nodes_temp;

    for (unsigned k = 0; k < g->first_out.size() - 1; ++k) {
        unsigned x = g->order[k];

        //node_map1.clear();
        //nodes_temp.clear();

        /*if (k % 1000 == 0 && k > 100000) {
            cout << "edges[x].size() " << edges[x].size() << "\n";
        }
        for (unsigned i = 0; i < edges[x].size(); ++i) {
            unsigned y = edges[x][i];
            if (!node_map1[y]) {
                node_map1[y] = true;
                nodes_temp.push_back(y);
            }
        }

        edges[x].clear();
        for (unsigned i = 0; i < nodes_temp.size(); ++i) {
            edges[x].push_back(nodes_temp[i]);
        }*/

        
        unsigned cc = 0;
        for (unsigned i = 0; i < edges[x].size(); ++i) {
            unsigned y1 = edges[x][i];

            if (g->rank[x] >= g->rank[y1]) {
                continue;
            }

            for (unsigned j = i + 1; j < edges[x].size(); ++j) {
                cc++;
                unsigned y2 = edges[x][j];

                if (g->rank[x] >= g->rank[y2]) continue;
                
                //edges[y1].push_back(y2);
                //edges[y2].push_back(y1);

                /*bool edge_found = false;
                for (unsigned i1 = 0; i1 < edges[y1].size(); ++i1) {
                    if (y2 == edges[y1][i1] || y2 == y1 || y2 == x) edge_found = true;
                }
                if (!edge_found) edges[y1].push_back(y2);

                edge_found = false;
                for (unsigned i1 = 0; i1 < edges[y2].size(); ++i1) {
                    if (y1 == edges[y2][i1] || y1 == y2 || y1 == x) edge_found = true;
                }
                if (!edge_found) edges[y2].push_back(y1);*/

				if (!edge_maps[y1][y2]) {
                    edge_maps[y1][y2] = true;
                    edges[y1].push_back(y2);
                }
                if (!edge_maps[y2][y1]) {
                    edge_maps[y2][y1] = true;
                    edges[y2].push_back(y1);
                }
            }
            //cout << edges[y1].size() << "\n";
        }

 //       if (k % 1000 == 0 && k > 100000) {
 //           cout << "cc "<<  cc << " " << edges[x].size() << "\n";
 //           cout << k << " " << edges[x].size() << " " << g->first_out.size() << "\n";
 //       }
    }

    unsigned counter = 0;
    vector<unsigned> temp_first_out;
    vector<unsigned> temp_head;

    temp_first_out.push_back(0);

    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        sort(edges[x].begin(), edges[x].end(), less_than_rank());

        for (unsigned i = 0; i < edges[x].size(); ++i) {
            //cout << i << " - \n";
            counter++;
            temp_head.push_back(edges[x][i]);
        }
        temp_first_out.push_back(counter);
    }

    g->first_out = temp_first_out;
    g->head = temp_head;

    //buildUndirectedGraph(g);
}


/*
    Requires set order and ranks for the nodes
*/
void contractor::contractNode(CCHGraph *g, unsigned x) {
    
}

vector<unsigned> contractor::backwardOrder(vector<unsigned> &order) {
    vector<unsigned> rank(order.size());
    for (unsigned i = 0; i < order.size(); ++i) {
        rank[order[i]] = i;
    }
    return rank;
}




contractor::contractor() {
	// TODO Auto-generated constructor stub

}

contractor::~contractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
