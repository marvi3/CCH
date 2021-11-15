#include "vector_io.h"
#include "constants.h"
#include "id_queue.h"
#include "timer.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>
#include "constants.h"

#include "graph.h"
#include "QueryManager.h"
#include "stdafx.h"


using namespace std;

void QueryManager::init() {
    for (unsigned x = 0; x < g.first_out_up.size() - 1; ++x) {
        d_up.push_back(inf_weight);
        predecessor_up.push_back(inf_weight);
    }
    for (unsigned x = 0; x < g.first_out_down.size() - 1; ++x) {
        d_down.push_back(inf_weight);
        predecessor_down.push_back(inf_weight);
    }
    for (unsigned i = 0; i < g.first_out_up.size(); ++i) {
        d_forward.push_back(inf_weight);
        d_backward.push_back(inf_weight);
    }
}

void QueryManager::initParents() {
    unsigned min_neighbour = 0;
    unsigned min_rank = 0;
    
    unsigned y = 0;
    
    for (unsigned x = 0; x < g.first_out_up.size() - 1; ++x) {
        min_neighbour = g.first_out_up.size();
        min_rank = g.first_out_up.size();
        for (unsigned out_arc = g.first_out_up[x]; out_arc < g.first_out_up[x+1]; ++out_arc) {
            //cout << out_arc << " < oa \n";
            y = g.head_up[out_arc];
            if (min_rank > g.rank[y] && g.rank[x] < g.rank[y]) {
                min_neighbour = y;
                min_rank = g.rank[y];
            }
        }
        //cout << x << " -> " << min_neighbour << "  rank: " << min_rank << "\n";
        parent_up.push_back(min_neighbour);
    }

    for (unsigned x = 0; x < g.first_out_down.size() - 1; ++x) {
        min_neighbour = g.first_out_down.size();
        min_rank = g.first_out_down.size();
        for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
            y = g.head_down[out_arc];
            if (min_rank > g.rank[y] && g.rank[x] < g.rank[y]) {
                min_neighbour = y;
                min_rank = g.rank[y];
            }
        }
        parent_down.push_back(min_neighbour);
    }
}


unsigned QueryManager::eliminationTreeQuery(unsigned s, unsigned t) {
    if (s == t) return 0;

    unsigned s_p = s;
    unsigned t_p = t;
    unsigned lca;
    
    unsigned d_min = inf_weight;
    unsigned z = 0;

    unsigned d_min_up = inf_weight;
    unsigned d_min_down = inf_weight;

    unsigned node_counter = 0;
    unsigned edge_counter = 0;
    
    unsigned counter = 0;

    while (true) {  //find lowest common ancestor
        node_counter++;
        edge_counter++;

        if (s_p == t_p) {
            lca = s_p;
            break;
        }
        else if (g.rank[s_p] < g.rank[t_p]) {
            s_p = parent_up[s_p];
            if (s_p >= g.rank.size() - 1) return inf_weight;
        }
        else if (g.rank[s_p] > g.rank[t_p]) {
            //cout << "--->" << parent_down[t_p];
            t_p = parent_down[t_p];
            if (t_p >= g.rank.size() - 1) return inf_weight;
        }
        //if (counter > 1000) return -1;
        //cout  << s_p << " " << g.rank[s_p] << " " << t_p << " " << g.rank[t_p] << "\n";
    }
    
    //cout << "\n from " << s << " to " << t << "\n lca " << lca << "\n";

    s_p = s;
    t_p = t;
    unsigned y = 0;
    unsigned x = 0;
    
    d_up[s] = 0;
    d_down[t] = 0;
    
//    for (unsigned x = 0; x < g.first_out_up.size() - 1; ++x) {
//        cout << "before d: " << x << " " << d_up[x] << "\n";
//    }
    
    while(true) {   //forward search to lca
        x = s_p;
        if (x == lca) break;
        node_counter++;

        for (unsigned out_arc = g.first_out_up[x]; out_arc < g.first_out_up[x+1]; ++out_arc) {
            y = g.head_up[out_arc];

            edge_counter++;
            if (g.rank[y] > g.rank[x]) {

                if (d_up[x] + g.weight_up[out_arc] < d_up[y]) {
                    d_up[y] = d_up[x] + g.weight_up[out_arc];
                    predecessor_up[y] = x;
                    //cout << "_ " << x << " (" << d_up[y] << ") -> " << y << "\n";
                }
                
                if (y == t) {
                    if (d_up[y] < d_min) {
                        d_min = d_up[y];
                        z = y;
                        predecessor_up[y] = x;
                        //cout << "-> " << d_min << "  " << z << "\n";
                    }
                }
            }
        }
        s_p = parent_up[s_p];
    }
    
    while(true) {   //backward search to lca
        x = t_p;
        if (x == lca) break;
        node_counter++;

        for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
            y = g.head_down[out_arc];

            edge_counter++;
            if (g.rank[y] > g.rank[x]) {

                if (d_down[x] + g.weight_down[out_arc] < d_down[y]) {
                    d_down[y] = d_down[x] + g.weight_down[out_arc];

                    predecessor_down[y] = x;
                }
                
                if (d_up[y] < inf_weight) {
                    if (d_up[y] + d_down[y] < d_min) {
                        d_min = d_up[y] + d_down[y];
                        z = y;
                    }
                }
                
                if (y == s) {
                    if (d_down[y] < d_min) {
                        d_min = d_down[y];
                        z = y;
                        predecessor_down[y] = x;
                    }
                }
            }
        }
        t_p = parent_down[t_p];
    }
    
    while(true) {   //forward search to root
        x = s_p;
        if (x >= g.first_out_up.size() - 1) break;
        node_counter++;

        for (unsigned out_arc = g.first_out_up[x]; out_arc < g.first_out_up[x+1]; ++out_arc) {
            y = g.head_up[out_arc];

            edge_counter++;
            if (g.rank[y] > g.rank[x]) {

                if (d_up[x] + g.weight_up[out_arc] < d_up[y]) {
                    d_up[y] = d_up[x] + g.weight_up[out_arc];
                    predecessor_up[y] = x;
                }
                
                if (d_down[y] < inf_weight) {
                    if (d_up[y] + d_down[y] < d_min) {
                        d_min = d_up[y] + d_down[y];
                        z = y;
                    }
                }
                
                if (y == t) {
                    if (d_up[y] < d_min) {
                        d_min = d_up[y];
                        z = y;
                        predecessor_up[y] = x;
                    }
                }
            }
        }
        s_p = parent_up[s_p];
    }
    
    while(true) {   //backward search to root
        x = t_p;
        if (x >= g.first_out_down.size() - 1) break;
        node_counter++;
        
        for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
            y = g.head_down[out_arc];

            edge_counter++;
            if (g.rank[y] > g.rank[x]) {

                if (d_down[x] + g.weight_down[out_arc] < d_down[y]) {
                    d_down[y] = d_down[x] + g.weight_down[out_arc];
                    predecessor_down[y] = x;
                }
                
                if (d_up[y] < inf_weight) {
                    if (d_up[y] + d_down[y] < d_min) {
                        d_min = d_up[y] + d_down[y];
                        z = y;
                    }
                }
                
                if (y == s) {
                    if (d_down[y] < d_min) {
                        d_min = d_down[y];
                        z = y;
                        predecessor_down[y] = x;
                    }
                }
            }
        }
        t_p = parent_down[t_p];
    }
    
    if (d_up[z] + d_down[z] < d_min) d_min = d_up[z] + d_down[z];

    last_collision_node = z;
    last_start_node = s;
    last_target_node = t;

    //reset
    s_p = s;
    t_p = t;
    
    d_up[s] = inf_weight;
    d_down[t] = inf_weight;
    
    while(true) {
        x = s_p;
        for (unsigned out_arc = g.first_out_up[x]; out_arc < g.first_out_up[x+1]; ++out_arc) {
            y = g.head_up[out_arc];
            d_up[y] = inf_weight;
            //predecessor_up[y] = inf_weight;
        }
        s_p = parent_up[s_p];
        
        if (s_p >= g.first_out_up.size() - 1) break;
    }
    while(true) {
        x = t_p;
        for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
            y = g.head_down[out_arc];
            d_down[y] = inf_weight;
            //predecessor_down[y] = inf_weight;
        }
        t_p = parent_down[t_p];
        
        if (t_p >= g.first_out_down.size() - 1) break;
    }

    visited_nodes.push_back(node_counter);
    visited_edges.push_back(edge_counter);

    //cout << "iterated nodes: " << node_counter << "\n";
    
    return d_min;
}

unsigned QueryManager::basicQuery(unsigned s, unsigned t) {
    if (s == t) return 0;
    auto vnum = g.first_out_up.size();

    unsigned edge_counter = 0;
    unsigned node_counter = 0; //used with visited_nodes


    auto q_forward = new MinIDQueue(vnum);
    auto q_backward = new MinIDQueue(vnum);

    for (unsigned i = 0; i < vnum; ++i) {
        d_forward[i] = inf_weight;
        d_backward[i] = inf_weight;
    }

    d_forward[s] = 0;
    d_backward[t] = 0;

    IDKeyPair idkp;
    idkp.id = s;
    idkp.key = 0;
    q_forward->clear();
    q_forward->push(idkp);

    idkp.id = t;
    idkp.key = 0;
    q_backward->clear();
    q_backward->push(idkp);

    unsigned min_dist = inf_weight;
    unsigned min_dist_forward = 0;
    unsigned min_dist_backward = 0;

    int x = 0;

    while (!q_forward->empty() || !q_backward->empty()) {
        if (!q_forward->empty()) {
            idkp = q_forward->pop();

            if (min_dist_forward > min_dist) continue;
            node_counter++;

            x = idkp.id;
            min_dist_forward = idkp.key;

            if (x == t && min_dist > idkp.key) return idkp.key;

            for (unsigned out_arc = g.first_out_up[x]; out_arc < g.first_out_up[x+1]; ++out_arc) {
                unsigned y = g.head_up[out_arc];

                if (g.rank[x] > g.rank[y]) continue;
                edge_counter++;

                if (d_forward[x] + g.weight_up[out_arc] < d_forward[y]) {
                    d_forward[y] = d_forward[x] + g.weight_up[out_arc];

                    if (q_forward->contains_id(y)) {
                        idkp.id = y;
                        idkp.key = d_forward[y];
                        q_forward->decrease_key(idkp);
                    }
                    else {
                        idkp.id = y;
                        idkp.key = d_forward[y];
                        q_forward->push(idkp);
                    }
                }

                if (min_dist > d_forward[y] + d_backward[y]) {
                    min_dist = d_forward[y] + d_backward[y];
                }
            }
        }

        if (!q_backward->empty()) {
            idkp = q_backward->pop();

            if (min_dist_backward > min_dist) continue;
            node_counter++;

            x = idkp.id;
            min_dist_backward = idkp.key;

            if (x == s && min_dist > idkp.key) return idkp.key;

            for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
                unsigned y = g.head_down[out_arc];

                if (g.rank[x] > g.rank[y]) continue;
                edge_counter++;

                if (d_backward[x] + g.weight_down[out_arc] < d_backward[y]) {
                    d_backward[y] = d_backward[x] + g.weight_down[out_arc];

                    if (q_backward->contains_id(y)) {
                        idkp.id = y;
                        idkp.key = d_backward[y];
                        q_backward->decrease_key(idkp);
                    }
                    else {
                        idkp.id = y;
                        idkp.key = d_backward[y];
                        q_backward->push(idkp);
                    }
                }

                if (min_dist > d_forward[y] + d_backward[y]) {
                    min_dist = d_forward[y] + d_backward[y];
                }
            }
        }

        if (min_dist_backward > min_dist && min_dist_forward > min_dist) break;
    }
    delete(q_forward);
    delete(q_backward);

    visited_nodes.push_back(node_counter);
    visited_edges.push_back(edge_counter);

    return min_dist;
}


vector<unsigned> QueryManager::unpack_path() {
    //path unpacking!!!
    unsigned l = last_collision_node;
    path_up.clear();
    path_down.clear();

    while (true) {
        if (l == inf_weight) break;
        path_up.push_back(l);
        if (l == last_start_node) break;
        l = predecessor_up[l];
    }
    l = last_collision_node;
    while (true) {
        if (l == inf_weight) break;
        path_down.push_back(l);
        if (l == last_target_node) break;
        l = predecessor_down[l];
    }


    //Triangles
    vector<vector<pair<unsigned, unsigned>>> n_down;
    vector<vector<pair<unsigned, unsigned>>> n_up;

    unsigned y;
    for (unsigned x = 0; x < g.first_out.size() - 1; ++x) {
        vector<pair<unsigned, unsigned>> temp;
        n_up.push_back(temp);
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x + 1]; ++out_arc) {
            y = g.head[out_arc];
            if (g.rank[x] < g.rank[y]) {
                n_up[x].push_back(pair<unsigned, unsigned> (y, out_arc));
            }
        }
        std::sort(n_up[x].begin(), n_up[x].end());
    }

    for (unsigned x = 0; x < g.first_out.size() - 1; ++x) {
        vector<pair<unsigned, unsigned>> temp;
        n_down.push_back(temp);
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x + 1]; ++out_arc) {
            y = g.head[out_arc];
            if (g.rank[x] > g.rank[y]) {
                n_down[x].push_back(pair<unsigned, unsigned>(y, out_arc));
            }
        }
        std::sort(n_down[x].begin(), n_down[x].end());
    }


    //Make contracted path

    vector<unsigned> temp_up;
    vector<unsigned> temp_down;

    cout << "pu: ";
    for (int x = path_up.size() - 1; x>=0; --x) {
        if (x >= inf_weight) break;
        cout << path_up[x] << " - ";
        temp_up.push_back(path_up[x]);
    }
    cout << "\n";

    cout << "pd: ";
    for (int x = 0; x < path_down.size(); ++x) {
        if (x >= inf_weight) break;
        cout << path_down[x] << " - ";
        temp_down.push_back(path_down[x]);
    }
    cout << "\n";

    /*for (unsigned x = 0; x < temp.size(); ++x) {
        cout << temp[x] << " - ";
    }
    cout << "\n";*/

    vector<pair<unsigned, unsigned>> isec;
    vector<unsigned> node_isec;

    while (true) {
        bool path_changed = false;
        for (unsigned i = 0; i < temp_up.size()-1; ++i) {
            unsigned x = temp_up[i];
            unsigned y = temp_up[i+1];
            
            unsigned out_arc = 0;
            for (unsigned j = g.first_out[x]; j < g.first_out[x+1]; ++j) {
                if (g.head[j] == y) out_arc = j;
            }

            isec.clear();
            node_isec.clear();

            unsigned x_i = 0;
            unsigned y_i = 0;

            if (n_down[x].size() == 0 || n_down[y].size() == 0) {
                continue;
            }

            while (true) {
                if (n_down[x][x_i].first == n_down[y][y_i].first) {
                    isec.push_back(n_down[x][x_i]);
					isec.push_back(n_down[y][y_i]);
                    node_isec.push_back(n_down[x][x_i].first);
					x_i++;
                    y_i++;
                }
                else if (n_down[x][x_i] < n_down[y][y_i]) {
                    x_i++;
                }
                else if (n_down[x][x_i] > n_down[y][y_i]) {
                    y_i++;
                }

                if (x_i >= n_down[x].size() || y_i >= n_down[y].size()) break;
            }


            for (unsigned j = 0; j < isec.size(); ++j) {
                unsigned z = node_isec[j/2];
                unsigned z1 = isec[j].second;
                j++;
                unsigned z2 = isec[j].second;
                unsigned zy_pos = g.reverse_head[z2];      

                //up graph
                unsigned xy = g.weight_up[out_arc];
                unsigned xz = g.weight_up[z1];
                unsigned zy = g.weight_up[zy_pos];

                if (xz + zy == xy) {
                    path_changed = true;
                    temp_up = insertNode(temp_up, x, z);
                }
            }
        }

        if (!path_changed) break;
    }

    cout << "up\n";

    /*cout << "\ntemp_up: ";
    for (int i = temp_up.size()-1; i >= 0; --i) {
        cout << temp_up[i] << " - ";
    }
    cout << "\n";*/

    while (true) {
        bool path_changed = false;
        for (unsigned i = 0; i < temp_down.size()-1; ++i) {
            unsigned x = temp_down[i];
            unsigned y = temp_down[i+1];

            unsigned out_arc = 0;
            for (unsigned j = g.first_out[x]; j < g.first_out[x+1]; ++j) {
                if (g.head[j] == y) out_arc = j;
            }

            isec.clear();
            node_isec.clear();

            unsigned x_i = 0;
            unsigned y_i = 0;

            if (n_down[x].size() == 0 || n_down[y].size() == 0) {
                continue;
            }

            while (true) {
                if (n_down[x][x_i].first == n_down[y][y_i].first) {
                    isec.push_back(n_down[x][x_i]);
					isec.push_back(n_down[y][y_i]);
                    node_isec.push_back(n_down[x][x_i].first);
					x_i++;
                    y_i++;
                }
                else if (n_down[x][x_i] < n_down[y][y_i]) {
                    x_i++;
                }
                else if (n_down[x][x_i] > n_down[y][y_i]) {
                    y_i++;
                }

                if (x_i >= n_down[x].size() || y_i >= n_down[y].size()) break;
            }


            for (unsigned j = 0; j < isec.size(); ++j) {
                unsigned z = node_isec[j/2];
                unsigned z1 = isec[j].second;
                j++;
                unsigned z2 = isec[j].second;
                unsigned zy_pos = g.reverse_head[z2]; 

                //down graph
                unsigned xy = g.weight_down[out_arc];
                unsigned xz = g.weight_down[z1];
                unsigned zy = g.weight_down[zy_pos];

                if (xz + zy == xy) {
                    path_changed = true;
                    temp_down = insertNode(temp_down, x, z);
                }
            }
        }

        if (!path_changed) break;
    }

    cout << "down\n";

    /*cout << "\ntemp_down: ";
    for (int i = temp_down.size()-1; i >= 0; --i) {
        cout << temp_down[i] << " - ";
    }
    cout << "\n";*/

    vector<unsigned> ret;

    //cout << "\ncust_path: ";
    for (unsigned i = 0; i < temp_up.size()-1; ++i) {
        //cout << temp_up[i] << " - ";
        ret.push_back(temp_up[i]);
    }
    //cout << temp_up[temp_up.size()-1] << " - ";

    for (unsigned i = 0; i < temp_down.size()-1; ++i) {
        //cout << temp_down[i] << " - ";
        ret.push_back(temp_down[i]);
    }
    //cout << temp_down[temp_down.size()-1] << "\n";
    cout << "ret\n";
    return ret;
}

vector<unsigned> QueryManager::insertNode(vector<unsigned> &v, unsigned x, unsigned z) {
    //cout << " insert " << x << " -> " << z <<"\n";
    vector<unsigned> temp;
    for (unsigned i = 0; i < v.size(); ++i) {
        unsigned n = v[i];
        temp.push_back(n);
        if (n == x) {
            temp.push_back(z);
        }
    }
    return temp;
}

unsigned QueryManager::getWeight(unsigned x, unsigned y, bool direction) {
    unsigned n;
    if (direction == 0) {  //up
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x+1]; ++out_arc) {
            n = g.head[out_arc];
            //cout << "  u " << x << " -> " << n << "\n";
            if (n == y) {
                return g.weight_up[out_arc];
            }
        }
    }
    else {  //down
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x+1]; ++out_arc) {
            n = g.head[out_arc];
            //cout << "  d " << x << " -> " << n << "\n";
            if (n == y) {
                return g.weight_down[out_arc];
            }
        }
    }
    cout << "can't find weight! " << x << " -> " << y << "\n";
    return inf_weight;
}

QueryManager::QueryManager() {

}
