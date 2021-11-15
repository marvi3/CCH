#include "vector_io.h"
#include "constants.h"
#include "id_queue.h"
#include "timer.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>

#include <algorithm>

#include "Customizer.h"
#include "CCHGraph.h"


using namespace std;



void Customizer::customize(CCHGraph *g, vector<map<unsigned, unsigned>> &w) {
    initMetric(g, w);
    initTriangles(g);
    
    applyBasicCustomization(g);
    /*applyBasicCustomization(g);
    applyBasicCustomization(g);
    applyBasicCustomization(g);
    applyBasicCustomization(g);
    applyBasicCustomization(g);*/
    //applyBasicCustomization_old(g);

    //applyPerfectCustomization(g);
    //applyPerfectCustomization_old(g);
}



void Customizer::initMetric(CCHGraph *g, vector<map<unsigned,unsigned>> &w) {
    unsigned y;
    g->weight_up.clear();
    g->weight_down.clear();
    g->weight_up.resize(g->head.size(), inf_weight);
    g->weight_down.resize(g->head.size(), inf_weight);

    unsigned weight;

    /*for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        cout << "cg " << x << ": ";
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x+1]; ++out_arc) {
            unsigned y = g->head[out_arc];
            cout << " " << y << "(" << g->weight[out_arc] << ")";
        }
        cout << "\n";
    }*/


    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x+1]; ++out_arc) {
            y = g->head[out_arc];
			    
            weight = find_weight(x, y, g, w);
            //if (x == 46 && y == 47) cout << "---weight " << weight << "\n";

            g->setWeight(x, y, weight, 0);
            g->setWeight(y, x, weight, 1);
        }
    }
}

unsigned Customizer::find_weight(unsigned x, unsigned y, CCHGraph *g, vector<map<unsigned, unsigned>> &w) {
    unsigned n = 0;
    auto it = w[x].find(y);

    if (it != w[x].end()) return it->second;

    return inf_weight;
    //return g->getWeight(x, y);
}


void Customizer::initTriangles(CCHGraph *g) {
    unsigned count = 0;
    unsigned c2 = 0;
    n_up_first.push_back(0);
    n_down_first.push_back(0);

    unsigned y;
    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        vector<unsigned> temp;
        n_up.push_back(temp);
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->backward_order[x] < g->backward_order[y]) {
                n_up[x].push_back(y);
            }
        }
        std::sort(n_up[x].begin(), n_up[x].end());
    }

    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        vector<unsigned> temp;
        n_down.push_back(temp);
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->backward_order[x] > g->backward_order[y]) {
                n_down[x].push_back(y);
            }
        }
        std::sort(n_down[x].begin(), n_down[x].end());
    }

}


void Customizer::applyBasicCustomization_old(CCHGraph *g) {
    
}



void Customizer::applyBasicCustomization(CCHGraph *g) {
    unsigned x,y;
    unsigned x_i, y_i;
    vector<unsigned> isec;

    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        x = g->order[i];
        
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
        //for (unsigned i2 = 0; i2 < g->first_out.size() - 1; ++i2) {
            isec.clear();
            y = g->head[out_arc];
            //y = g->order[i2];
            /*bool edge_found = false;
            for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
                if (y == g->head[out_arc]) {
                    edge_found = true;
                }
            }
            if (!edge_found) continue;*/

            //if (g->backward_order[x] >= g->backward_order[y]) continue;

            x_i = 0;
            y_i = 0;

            if (n_down[x].size() == 0 || n_down[y].size() == 0) {
                continue;
            }

            while (true) {
                if (n_down[x][x_i] == n_down[y][y_i]) {
                    isec.push_back(n_down[x][x_i]);
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
                unsigned z = isec[j];

                //up graph


                /*  xy: 46,23 - 955    xz: 46,15 - 496    zy: 15,23 - 153
                 *  xy: 46,15 - 2147483647    xz: 46,24 - 120    zy: 24,15 - 376
                 *


                46(91) - 47(349) - 44(72) - 43(113) - 23

                */


                unsigned xy = g->getWeight(x, y, 0);
                unsigned xz = g->getWeight(x, z, 0);
                unsigned zy = g->getWeight(z, y, 0);
                /*if (x == 46 || x == 47 || x == 44 || x == 43 || x ==23) {
                    cout << "xy: " << x << "," << y << " - " << xy
                         << "    xz: " << x << "," << z << " - " << xz
                         << "    zy: " << z << "," << y << " - " << zy << "\n";
                }*/
                if (xz + zy < xy) {

                    g->setWeight(x, y, xz + zy, 0);
                }

                //down graph
                xy = g->getWeight(x, y, 1);
                xz = g->getWeight(x, z, 1);
                zy = g->getWeight(z, y, 1);
                if (xz + zy < xy) {
                    g->setWeight(x, y, xz + zy, 1);
                }
            }
        }
    }

    cout << "size before:" << g->head.size() << "\n";

    vector<unsigned> temp_fo, temp_head, temp_weight;
    unsigned counter = 0;

    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        temp_fo.push_back(counter);
        for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
            y = g->head[out_arc];

            temp_head.push_back(y);
            temp_weight.push_back(g->weight_up[out_arc]);
            counter++;
        }
    }
    temp_fo.push_back(counter);

    copyVector(&g->first_out_up, &temp_fo);
    copyVector(&g->head_up, &temp_head);
    copyVector(&g->weight_up, &temp_weight);

    cout << "size after:" << g->head_up.size() << "\n";

    temp_fo.clear();
    temp_head.clear();
    temp_weight.clear();
    counter = 0;
    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        temp_fo.push_back(counter);
        for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
            y = g->head[out_arc];

            temp_head.push_back(y);
            temp_weight.push_back(g->weight_down[out_arc]);
            counter++;
        }
    }
    temp_fo.push_back(counter);

    copyVector(&g->first_out_down, &temp_fo);
    copyVector(&g->head_down, &temp_head);
    copyVector(&g->weight_down, &temp_weight);

    /*for (unsigned x = 0; x < g->first_out_up.size() - 1; ++x) {
        cout << "bg " << x << ": ";
        for (unsigned out_arc = g->first_out_up[x]; out_arc < g->first_out_up[x+1]; ++out_arc) {
            unsigned y = g->head_up[out_arc];
            cout << " " << y << "(" << g->weight_up[out_arc] << ")";
        }
        cout << "\n";
    }*/
}



void Customizer::applyPerfectCustomization_old(CCHGraph *g) {  //todo: cleanup
   
}



void Customizer::applyPerfectCustomization(CCHGraph *g) {
    vector<unsigned> isec;
    unsigned x_i, y_i = 0;

    vector<unsigned> changed_up;
    vector<unsigned> changed_down;

    for (unsigned i = g->order.size() - 1; i >= 0; --i) {
        if (i >= g->order.size()) break;

        unsigned x = g->order[i];
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            unsigned y = g->head[out_arc];

            isec.clear();

            x_i = 0;
            y_i = 0;
            while (true) {
                if (n_up[x].size() == 0 || n_up[y].size() == 0) break;

                if (n_up[x][x_i] == n_up[y][y_i]) {
                    isec.push_back(n_up[x][x_i]);
                    x_i++;
                    y_i++;
                }
                else if (n_up[x][x_i] < n_up[y][y_i]) {
                    x_i++;
                }
                else if (n_up[x][x_i] > n_up[y][y_i]) {
                    y_i++;
                }

                if (x_i >= n_up[x].size() || y_i >= n_up[y].size()) break;
            }

            x_i = 0;
            y_i = 0;
            while (true) {
                if (n_up[x].size() == 0 || n_down[y].size() == 0) break;

                if (n_up[x][x_i] == n_down[y][y_i]) {
                    isec.push_back(n_up[x][x_i]);
                    x_i++;
                    y_i++;
                }
                else if (n_up[x][x_i] < n_down[y][y_i]) {
                    x_i++;
                }
                else if (n_up[x][x_i] > n_down[y][y_i]) {
                    y_i++;
                }

                if (x_i >= n_up[x].size() || y_i >= n_down[y].size()) break;
            }

            x_i = 0;
            y_i = 0;
            while (true) {
                if (n_down[x].size() == 0 || n_up[y].size() == 0) break;

                if (n_down[x][x_i] == n_up[y][y_i]) {
                        isec.push_back(n_down[x][x_i]);
                        x_i++;
                        y_i++;
                }
                else if (n_down[x][x_i] < n_up[y][y_i]) {
                        x_i++;
                }
                else if (n_down[x][x_i] > n_up[y][y_i]) {
                        y_i++;
                }

                if (x_i >= n_down[x].size() || y_i >= n_up[y].size()) break;
            }

            for (unsigned j = 0; j < isec.size(); ++j) {
                unsigned z = isec[j];

                //up graph
                unsigned xy = g->getWeight(x, y, 0);
                unsigned xz = g->getWeight(x, z, 0);
                unsigned zy = g->getWeight(z, y, 0);
                if (xz + zy < xy) {
                    g->setWeight(x, y, xz + zy, 0);
                    changed_up.push_back(x);
                    changed_up.push_back(y);
                }

                //down graph
                xy = g->getWeight(x, y, 1);
                xz = g->getWeight(x, z, 1);
                zy = g->getWeight(z, y, 1);
                if (xz + zy < xy) {
                    g->setWeight(x, y, xz + zy, 1);
                    changed_down.push_back(x);
                    changed_down.push_back(y);
                }
            }
        }
    }

    for (unsigned i = 0; i < changed_up.size() - 1; i=i+2) {
        unsigned x1 = changed_up[i];
        unsigned y1 = changed_up[i+1];
        //g->setWeight(x1,y1,-1,0);
    }


    cout << "size after:" << g->head_up.size() << "\n";

    unsigned y = 0;
    vector<unsigned> temp_fo, temp_head, temp_weight;
    unsigned counter = 0;

    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        temp_fo.push_back(counter);
        for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->weight_up[out_arc] <= inf_weight && g->weight_up[out_arc] != -1) {
                temp_head.push_back(y);
                temp_weight.push_back(g->weight_up[out_arc]);
                counter++;
            }
        }
    }
    temp_fo.push_back(counter);

    copyVector(&g->first_out_up, &temp_fo);
    copyVector(&g->head_up, &temp_head);
    copyVector(&g->weight_up, &temp_weight);

    cout << "size after:" << g->head_up.size() << "\n";

    temp_fo.clear();
    temp_head.clear();
    temp_weight.clear();
    counter = 0;
    for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        temp_fo.push_back(counter);
        for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->weight_down[out_arc] <= inf_weight && g->weight_down[out_arc] != -1) {
                temp_head.push_back(y);
                temp_weight.push_back(g->weight_down[out_arc]);
                counter++;
            }
        }
    }
    temp_fo.push_back(counter);

    copyVector(&g->first_out_down, &temp_fo);
    copyVector(&g->head_down, &temp_head);
    copyVector(&g->weight_down, &temp_weight);
}

void renewWeight(CCHGraph *g, unsigned x, unsigned y, unsigned z) {

}



void Customizer::copyVector(vector<unsigned> *v1, vector<unsigned> *v2) {
    v1->clear();
    
    for (unsigned i = 0; i < v2->size(); ++i) {
        v1->push_back((*v2)[i]);
        //cout << "    -" << (*v2)[i] << " \n";
    }
}


vector<unsigned> Customizer::intersection(vector<unsigned> &v1, vector<unsigned> &v2){
    vector<unsigned> v3;

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

/*    set_intersection(v1.begin(),v1.end(),
                     v2.begin(),v2.end(),
                     back_inserter(v3));*/
    return v3;
}
