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
            if (min_rank > g.backward_order[y] && g.backward_order[x] < g.backward_order[y]) {
                min_neighbour = y;
                min_rank = g.backward_order[y];
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
            if (min_rank > g.backward_order[y] && g.backward_order[x] < g.backward_order[y]) {
                min_neighbour = y;
                min_rank = g.backward_order[y];
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
    
    unsigned counter = 0;

    while (true) {  //find lowest common ancestor
        node_counter++;
        if (s_p == t_p) {
            lca = s_p;
            break;
        }
        else if (g.backward_order[s_p] < g.backward_order[t_p]) {
            s_p = parent_up[s_p];
            if (s_p >= g.backward_order.size() - 1) return -1;
        }
        else if (g.backward_order[s_p] > g.backward_order[t_p]) {
            //cout << "--->" << parent_down[t_p];
            t_p = parent_down[t_p];
            if (t_p >= g.backward_order.size() - 1) return -1;
        }
        //if (counter > 1000) return -1;
        //cout  << s_p << " " << g.backward_order[s_p] << " " << t_p << " " << g.backward_order[t_p] << "\n";
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
            node_counter++;
            if (g.backward_order[y] > g.backward_order[x]) {
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
            node_counter++;

            if (g.backward_order[y] > g.backward_order[x]) {
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
            node_counter++;

            if (g.backward_order[y] > g.backward_order[x]) {
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
            node_counter++;

            if (g.backward_order[y] > g.backward_order[x]) {
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
            predecessor_up[y] = inf_weight;
        }
        s_p = parent_up[s_p];
        
        if (s_p >= g.first_out_up.size() - 1) break;
    }
    while(true) {
        x = t_p;
        for (unsigned out_arc = g.first_out_down[x]; out_arc < g.first_out_down[x+1]; ++out_arc) {
            y = g.head_down[out_arc];
            d_down[y] = inf_weight;
            predecessor_down[y] = inf_weight;
        }
        t_p = parent_down[t_p];
        
        if (t_p >= g.first_out_down.size() - 1) break;
    }

    //cout << "iterated nodes: " << node_counter << "\n";
    
    return d_min;
}

vector<unsigned> QueryManager::unpack_path() {
    //path unpacking!!!
    unsigned l = last_collision_node;
    while (true) {
        if (l == inf_weight) break;
        path_up.push_back(l);
        if (l == s) break;
        l = predecessor_up[l];
    }
    l = z;
    while (true) {
        if (l == inf_weight) break;
        path_down.push_back(l);
        if (l == t) break;
        l = predecessor_down[l];
    }


    //Triangles
    vector<unsigned> n_up_first;
    vector<unsigned> n_down_first;
    vector<unsigned> n_up_head;
    vector<unsigned> n_down_head;

    n_up_first.push_back(0);
    n_down_first.push_back(0);

    unsigned count = 0;

    for (unsigned x = 0; x < g.first_out.size() - 1; ++x) {
        unsigned c2 = count;
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x+1]; ++out_arc) {

            if (g.backward_order[x] < g.backward_order[g.head[out_arc]]) {
                n_up_head.push_back(g.head[out_arc]);
                count += 1;
            }
        }
        n_up_first.push_back(count);

        sort(n_up_head.begin() + c2, n_up_head.begin() + count);
    }

    count = 0;
    for (unsigned x = 0; x < g.first_out.size() - 1; ++x) {
        unsigned c2 = count;
        for (unsigned out_arc = g.first_out[x]; out_arc < g.first_out[x+1]; ++out_arc) {
            if (g.backward_order[x] > g.backward_order[g.head[out_arc]]) {
                n_down_head.push_back(g.head[out_arc]);
                count += 1;
            }
        }
        n_down_first.push_back(count);

        sort(n_down_head.begin() + c2, n_down_head.begin() + count);
    }
    n_down_first.push_back(count);


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

    vector<unsigned> isec;

    while (true) {
        bool path_changed = false;
        for (unsigned i = 0; i < temp_up.size()-1; ++i) {
            unsigned x = temp_up[i];
            unsigned y = temp_up[i+1];

            isec.clear();

            unsigned i1 = n_down_first[x];
            unsigned i2 = n_down_first[y];

            while(true) {
                if (i1 >= n_down_first[x+1] || i2 >= n_down_first[y+1]) {
                    break;
                }
                if (n_down_head[i1] < n_down_head[i2]) {
                    i1++;
                } else if (n_down_head[i1] > n_down_head[i2]) {
                    i2++;
                } else {
                    isec.push_back(n_down_head[i1]);
                    i1++;
                    i2++;
                }
            }

            for (unsigned j = 0; j < isec.size(); ++j) {
                unsigned z = isec[j];             

                //up graph
                unsigned xy = g.getWeight(x, y, 0);
                unsigned xz = g.getWeight(x, z, 0);
                unsigned zy = g.getWeight(z, y, 0);
                if (xz + zy == xy) {
                    path_changed = true;
                    temp_up = insertNode(temp_up, x, z);
                }
            }
        }

        if (!path_changed) break;
    }

    cout << "\ntemp_up: ";
    for (int i = temp_up.size()-1; i >= 0; --i) {
        cout << temp_up[i] << " - ";
    }
    cout << "\n";

    while (true) {
        bool path_changed = false;
        for (unsigned i = 0; i < temp_down.size()-1; ++i) {
            unsigned x = temp_down[i];
            unsigned y = temp_down[i+1];

            isec.clear();

            unsigned i1 = n_down_first[x];
            unsigned i2 = n_down_first[y];

            while(true) {
                if (i1 >= n_down_first[x+1] || i2 >= n_down_first[y+1]) {
                    break;
                }
                if (n_down_head[i1] < n_down_head[i2]) {
                    i1++;
                } else if (n_down_head[i1] > n_down_head[i2]) {
                    i2++;
                } else {
                    isec.push_back(n_down_head[i1]);
                    i1++;
                    i2++;
                }
            }

            for (unsigned j = 0; j < isec.size(); ++j) {
                unsigned z = isec[j];

                //down graph
                unsigned xy = g.getWeight(x, y, 1);
                unsigned xz = g.getWeight(x, z, 1);
                unsigned zy = g.getWeight(z, y, 1);
                if (xz + zy == xy) {
                    path_changed = true;
                    temp_down = insertNode(temp_down, x, z);
                }
            }
        }

        if (!path_changed) break;
    }
    cout << "\ntemp_down: ";
    for (int i = temp_down.size()-1; i >= 0; --i) {
        cout << temp_down[i] << " - ";
    }
    cout << "\n";

    vector<unsigned> ret;

    cout << "\ncust_path: ";
    for (unsigned i = 0; i < temp_up.size()-1; ++i) {
        cout << temp_up[i] << "(" << g.getWeight(temp_up[i], temp_up[i+1], 0) << ") - ";
        ret.push_back(temp_up[i]);
    }
    cout << temp_up[temp_up.size()-1] << " - ";

    for (unsigned i = 0; i < temp_down.size()-1; ++i) {
        cout << temp_down[i] << "(" << g.getWeight(temp_down[i], temp_down[i+1], 0) << ") - ";
        ret.push_back(temp_down[i]);
    }
    cout << temp_down[temp_down.size()-1] << "\n";

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
