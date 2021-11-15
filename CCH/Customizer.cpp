#include "vector_io.h"
#include "constants.h"
#include "id_queue.h"
#include "timer.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <map>
#include <string>
#include <chrono>

#include <algorithm>

#include "Customizer.h"
#include "CCHGraph.h"


using namespace std;



void Customizer::customize(CCHGraph *g, vector<map<unsigned, unsigned>> &w) {
    initMetric(g, w);
    
    applyCustomization(g);
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

            g->weight_up[out_arc] = weight;
			g->weight_down[g->reverse_head[out_arc]] = weight;
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
    n_up_first.push_back(0);
    n_down_first.push_back(0);

    unsigned y;
    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        vector<pair<unsigned, unsigned>> temp;
        n_up.push_back(temp);
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->rank[x] < g->rank[y]) {
                n_up[x].push_back(pair<unsigned, unsigned> (y, out_arc));
            }
        }
        std::sort(n_up[x].begin(), n_up[x].end());
    }

    for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
        vector<pair<unsigned, unsigned>> temp;
        n_down.push_back(temp);
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            y = g->head[out_arc];
            if (g->rank[x] > g->rank[y]) {
                n_down[x].push_back(pair<unsigned, unsigned>(y, out_arc));
            }
        }
        std::sort(n_down[x].begin(), n_down[x].end());
    }

}


void Customizer::initTrianglesCompletely(CCHGraph *g) {
	n_up_first.push_back(0);
	n_down_first.push_back(0);

	unsigned y;
	for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
		vector<pair<unsigned, unsigned>> temp;
		n_up.push_back(temp);
		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
			y = g->head[out_arc];
			if (g->rank[x] < g->rank[y]) {
				n_up[x].push_back(pair<unsigned, unsigned>(y, out_arc));
			}
		}
		std::sort(n_up[x].begin(), n_up[x].end());
	}

	for (unsigned x = 0; x < g->first_out.size() - 1; ++x) {
		vector<pair<unsigned, unsigned>> temp;
		n_down.push_back(temp);
		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
			y = g->head[out_arc];
			if (g->rank[x] > g->rank[y]) {
				n_down[x].push_back(pair<unsigned, unsigned>(y, out_arc));
			}
		}
		std::sort(n_down[x].begin(), n_down[x].end());
	}

	//lower triangles
	tri_down.clear();
	tri_down.resize(g->head.size());

	for (unsigned i = 0; i < g->first_out.size() - 1; i++) {
		unsigned x = g->order[i];
		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; out_arc++) {
			tri_down[out_arc] = vector<unsigned>();
			unsigned y = g->head[out_arc];
			unsigned x_i = 0;
			unsigned y_i = 0;
			
			if (n_down[x].size() == 0 || n_down[y].size() == 0) {
				continue;
			}

			while (true) {
				if (n_down[x][x_i].first == n_down[y][y_i].first) {
					tri_down[out_arc].push_back(n_down[x][x_i].second);
					tri_down[out_arc].push_back(n_down[y][y_i].second);
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
		}
	}

	//upper triangles
	tri_up.clear();
	tri_up.resize(g->head.size());

	tri_mid.clear();
	tri_mid.resize(g->head.size());

	for (unsigned i = g->order.size() - 1; i >= 0; --i) {
		if (i >= g->order.size()) break;
		unsigned x = g->order[i];
		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
			tri_up[out_arc] = vector<unsigned>();
			tri_mid[out_arc] = vector<unsigned>();
			unsigned y = g->head[out_arc];

			
			unsigned x_i = 0;
			unsigned y_i = 0;
			while (true) {
				if (n_up[x].size() == 0 || n_up[y].size() == 0) break;

				if (n_up[x][x_i].first == n_up[y][y_i].first) {
					tri_up[out_arc].push_back(n_up[x][x_i].second);
					tri_up[out_arc].push_back(n_up[y][y_i].second);
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

				if (n_up[x][x_i].first == n_down[y][y_i].first) {
					tri_mid[out_arc].push_back(n_up[x][x_i].second);
					tri_mid[out_arc].push_back(n_down[y][y_i].second);
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

				if (n_down[x][x_i].first == n_up[y][y_i].first) {
					tri_mid[out_arc].push_back(n_down[x][x_i].second);
					tri_mid[out_arc].push_back(n_up[y][y_i].second);
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
		}
	}
	triangles_initialized = true;
}

void Customizer::applyCustomization(CCHGraph *g) {
	if (triangles_initialized) {
		auto t_bc_0 = std::chrono::high_resolution_clock::now();
		applyBasicCustomizationTri(g);
		auto t_bc_1 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double>time_basic_customization = std::chrono::duration_cast<std::chrono::duration<double>>(t_bc_1 - t_bc_0);
		cout << "     The basic customization took " << time_basic_customization.count() << " seconds \n";
		
		if (g->perf_cust) {
			auto t_pc_0 = std::chrono::high_resolution_clock::now();
			applyPerfectCustomizationTri(g);
			auto t_pc_1 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double>time_perfect_customization = std::chrono::duration_cast<std::chrono::duration<double>>(t_pc_1 - t_pc_0);
			cout << "     The perfect customization took " << time_perfect_customization.count() << " seconds \n";
		}
			}
	else {
		auto t_bc_0 = std::chrono::high_resolution_clock::now();
		applyBasicCustomization(g);
		auto t_bc_1 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double>time_basic_customization = std::chrono::duration_cast<std::chrono::duration<double>>(t_bc_1 - t_bc_0);
		cout << "     The basic customization took " << time_basic_customization.count() << " seconds \n";

		if (g->perf_cust) {
			auto t_pc_0 = std::chrono::high_resolution_clock::now();
			applyPerfectCustomization(g);
			auto t_pc_1 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double>time_perfect_customization = std::chrono::duration_cast<std::chrono::duration<double>>(t_pc_1 - t_pc_0);
			cout << "     The perfect customization took " << time_perfect_customization.count() << " seconds \n";
		}
	}
}

void Customizer::applyBasicCustomization(CCHGraph *g) {
    unsigned x,y;
    unsigned x_i, y_i;
    vector<pair<unsigned, unsigned>> isec;

	std::chrono::duration<double> time_set_weights;

	for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
        x = g->order[i];

        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
        //for (unsigned i2 = 0; i2 < g->first_out.size() - 1; ++i2) {
            isec.clear();
            y = g->head[out_arc];
            //y = g->order[i2];

            //if (g->backward_order[x] >= g->backward_order[y]) continue;

            x_i = 0;
            y_i = 0;

            if (n_down[x].size() == 0 || n_down[y].size() == 0) {
                continue;
            }

            while (true) {
                if (n_down[x][x_i].first == n_down[y][y_i].first) {
                    isec.push_back(n_down[x][x_i]);
					isec.push_back(n_down[y][y_i]);
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
			
			auto t_sw_0 = std::chrono::high_resolution_clock::now();

            for (unsigned j = 0; j < isec.size(); j++) {
                unsigned xz_pos = isec[j].second;
				j++;
				unsigned zy_pos = g->reverse_head[isec[j].second];

                //up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[xz_pos];
				unsigned zy = g->weight_up[zy_pos];
				
				g->weight_up[out_arc] = min(xz + zy, xy);

				//down graph
				xy = g->weight_down[out_arc];
				xz = g->weight_down[xz_pos];
				zy = g->weight_down[zy_pos];

				g->weight_down[out_arc] = min(xz + zy, xy);
            }
			auto t_sw_1 = std::chrono::high_resolution_clock::now();
			time_set_weights += std::chrono::duration_cast<std::chrono::duration<double>>(t_sw_1 - t_sw_0);
		}
    }
	
	
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

    g->first_out_up = temp_fo;
    g->head_up = temp_head;
    g->weight_up = temp_weight;

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

    g->first_out_down = temp_fo;
    g->head_down = temp_head;
    g->weight_down = temp_weight;

    /*for (unsigned x = 0; x < g->first_out_up.size() - 1; ++x) {
        cout << "bg " << x << ": ";
        for (unsigned out_arc = g->first_out_up[x]; out_arc < g->first_out_up[x+1]; ++out_arc) {
            unsigned y = g->head_up[out_arc];
            cout << " " << y << "(" << g->weight_up[out_arc] << ")";
        }
        cout << "\n";
    }*/
}

void Customizer::applyBasicCustomizationTri(CCHGraph *g) {
	unsigned x, y;
	std::chrono::duration<double> time_set_weights;

	for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
		x = g->order[i];

		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
			//for (unsigned i2 = 0; i2 < g->first_out.size() - 1; ++i2) {
			y = g->head[out_arc];
			//y = g->order[i2];

			//if (g->backward_order[x] >= g->backward_order[y]) continue;

			if (tri_down[out_arc].size() == 0) {
				continue;
			}

			auto t_sw_0 = std::chrono::high_resolution_clock::now();

			for (unsigned j = 0; j < tri_down[out_arc].size(); j++) {
				unsigned xz_pos = tri_down[out_arc][j];
				j++;
				unsigned zy_pos = g->reverse_head[tri_down[out_arc][j]];

				//up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[xz_pos];
				unsigned zy = g->weight_up[zy_pos];

				g->weight_up[out_arc] = min(xz + zy, xy);

				//down graph
				xy = g->weight_down[out_arc];
				xz = g->weight_down[xz_pos];
				zy = g->weight_down[zy_pos];

				g->weight_down[out_arc] = min(xz + zy, xy);
			}
			auto t_sw_1 = std::chrono::high_resolution_clock::now();
			time_set_weights += std::chrono::duration_cast<std::chrono::duration<double>>(t_sw_1 - t_sw_0);
		}
	}

	
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

	g->first_out_up = temp_fo;
	g->head_up = temp_head;
	g->weight_up = temp_weight;

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

	g->first_out_down = temp_fo;
	g->head_down = temp_head;
	g->weight_down = temp_weight;
}

void Customizer::applyPerfectCustomization(CCHGraph *g) {
    vector<unsigned> isec_up;
	vector<unsigned> isec_mid;
	unsigned x_i, y_i = 0;
	vector<bool> changed_up(g->head.size(), false);
    vector<bool> changed_down(g->head.size(), false);

    for (unsigned i = g->order.size() - 1; i >= 0; --i) {
        if (i >= g->order.size()) break;
        unsigned x = g->order[i];
        for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
            unsigned y = g->head[out_arc];

			if (g->rank[y] < g->rank[x]) continue;

            isec_up.clear();
			isec_mid.clear();

            x_i = 0;
            y_i = 0;
            while (true) {
				if (n_up[x].size() == 0 || n_up[y].size() == 0) {
					break;
				}
                if (n_up[x][x_i].first == n_up[y][y_i].first) {
					isec_up.push_back(n_up[x][x_i].second);
					isec_up.push_back(n_up[y][y_i].second);
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

                if (n_up[x][x_i].first == n_down[y][y_i].first) {
					isec_mid.push_back(n_up[x][x_i].second);
					isec_mid.push_back(n_down[y][y_i].second);
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

                if (n_down[x][x_i].first == n_up[y][y_i].first) {
					isec_mid.push_back(n_down[x][x_i].second);
					isec_mid.push_back(n_up[y][y_i].second);
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

			for (unsigned j = 0; j < isec_mid.size(); ++j) {
				unsigned xz_pos = isec_mid[j];
				j++;
				unsigned zy_pos = g->reverse_head[isec_mid[j]];

				//up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[xz_pos];
				unsigned zy = g->weight_up[zy_pos];

				if (xz + zy < xy) {
					g->weight_up[out_arc] = xz + zy;
					changed_up[out_arc] = true;
				}


				//down graph
				xy = g->weight_down[g->reverse_head[out_arc]];
				xz = g->weight_down[g->reverse_head[xz_pos]];
				zy = g->weight_down[g->reverse_head[zy_pos]];
				if (xz + zy < xy) {
					g->weight_down[g->reverse_head[out_arc]] = xz + zy;
					changed_down[g->reverse_head[out_arc]] = true;
				}
			}
			for (unsigned j = 0; j < isec_up.size(); ++j) {
				unsigned xz_pos = isec_up[j];
				j++;
				unsigned zy_pos = g->reverse_head[isec_up[j]];

				//up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[xz_pos];
				unsigned zy = g->weight_up[zy_pos];

				if (xz + zy < xy) {
					g->weight_up[out_arc] = xz + zy;
					changed_up[out_arc] = true;
				}


				//down graph
				xy = g->weight_down[g->reverse_head[out_arc]];
				xz = g->weight_down[g->reverse_head[xz_pos]];
				zy = g->weight_down[g->reverse_head[zy_pos]];
				if (xz + zy < xy) {
					g->weight_down[g->reverse_head[out_arc]] = xz + zy;
					changed_down[g->reverse_head[out_arc]] = true;
				}
			}
        }
    }

	for (unsigned i = 0; i < changed_up.size(); i++) {
		if (changed_up[i]) {
			g->weight_up[i] = inf_weight;
		}
		//g->setWeight(x1,y1,-1,0);
	}
	for (unsigned i = 0; i < changed_down.size(); i++) {
		if (changed_down[i]) {
			g->weight_down[i] = inf_weight;
		}//g->setWeight(x1,y1,-1,0);
	}


		cout << "          Size before perfect customization: " << g->head_up.size() << "\n";

	vector<unsigned> temp_fo, temp_head, temp_weight;
	unsigned counter = 0;
	temp_fo.clear();
	temp_head.clear();
	temp_weight.clear();

	for (unsigned i = 0; i < g->first_out_up.size() - 1; ++i) {
		temp_fo.push_back(counter);
		for (unsigned out_arc = g->first_out_up[i]; out_arc < g->first_out_up[i + 1]; ++out_arc) {
			unsigned y = g->head_up[out_arc];
			if (!changed_up[out_arc]) {
				temp_head.push_back(y);
				temp_weight.push_back(g->weight_up[out_arc]);
				counter++;
			}
		}
	}

	temp_fo.push_back(counter);
	g->first_out_up = temp_fo;
	g->head_up = temp_head;
	g->weight_up = temp_weight;

	counter = 0;
	temp_fo.clear();
	temp_head.clear();
	temp_weight.clear();

	for (unsigned i = 0; i < g->first_out_down.size() - 1; ++i) {
		temp_fo.push_back(counter);
		for (unsigned out_arc = g->first_out_down[i]; out_arc < g->first_out_down[i + 1]; ++out_arc) {
			unsigned y = g->head_down[out_arc];
			if (!changed_down[out_arc]) {
				temp_head.push_back(y);
				temp_weight.push_back(g->weight_down[out_arc]);
				counter++;
			}
		}
	}

	temp_fo.push_back(counter);
	g->first_out_down = temp_fo;
	g->head_down = temp_head;
	g->weight_down = temp_weight;

	cout << "          Up-size after after perfect customization: " << g->head_up.size() << "\n";

	cout << "          Down-size after after perfect customization: " << g->head_down.size() << "\n";
}

void Customizer::applyPerfectCustomizationTri(CCHGraph *g) {
	vector<bool> changed_up(g->head.size(), false);
	vector<bool> changed_down(g->head.size(), false);

	for (unsigned i = g->order.size() - 1; i >= 0; --i) {
		if (i >= g->order.size()) break;
		unsigned x = g->order[i];
		for (unsigned out_arc = g->first_out[x]; out_arc < g->first_out[x + 1]; ++out_arc) {
			unsigned y = g->head[out_arc];
			if (tri_mid[out_arc].size() == 0) {
				continue;
			}
			//upper triangles
			for (unsigned j = 0; j < tri_mid[out_arc].size(); ++j) {
				unsigned xz_pos = tri_mid[out_arc][j];
				j++;
				unsigned zy_pos = g->reverse_head[tri_mid[out_arc][j]];

				//up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[xz_pos];
				unsigned zy = g->weight_up[zy_pos];

				if (xz + zy < xy) {
					g->weight_up[out_arc] = xz + zy;
					changed_up[out_arc] = true;
				}


				//down graph
				xy = g->weight_down[out_arc];
				xz = g->weight_down[xz_pos];
				zy = g->weight_down[zy_pos];
				if (xz + zy < xy) {
					g->weight_down[out_arc] = xz + zy;
					changed_down[out_arc] = true;
				}
			}

			//mid-triangles
			for (unsigned j = 0; j < tri_up[out_arc].size(); ++j) {
				unsigned z1 = tri_up[out_arc][j];
				j++;
				unsigned zy_pos = g->reverse_head[tri_up[out_arc][j]];

				//up graph
				unsigned xy = g->weight_up[out_arc];
				unsigned xz = g->weight_up[z1];
				unsigned zy = g->weight_up[zy_pos];

				if (xz + zy < xy) {
					g->weight_up[out_arc] = xz + zy;
					changed_up[out_arc] = true;
				}


				//down graph
				xy = g->weight_down[out_arc];
				xz = g->weight_down[z1];
				zy = g->weight_down[zy_pos];
				if (xz + zy < xy) {
					g->weight_down[out_arc] = xz + zy;
					changed_down[out_arc] = true;
				}
			}
		}
	}

//	for (unsigned i = 0; i < changed_up.size(); i++) {
//		g->weight_up[changed_up[i]] = inf_weight;
//		//g->setWeight(x1,y1,-1,0);
//	}
//	for (unsigned i = 0; i < changed_down.size(); i++) {
//		g->weight_down[changed_down[i]] = inf_weight;
//		//g->setWeight(x1,y1,-1,0);
//	}


	cout << "          Size before perfect customization: " << g->head_up.size() << "\n";

	vector<unsigned> temp_fo, temp_head, temp_weight;
	unsigned counter = 0;
	temp_fo.clear();
	temp_head.clear();
	temp_weight.clear();

	for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
		temp_fo.push_back(counter);
		for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
			unsigned y = g->head[out_arc];
			if (!changed_up[out_arc]) {
				temp_head.push_back(y);
				temp_weight.push_back(g->weight_up[out_arc]);
				counter++;
			}
		}
	}

	temp_fo.push_back(counter);
	g->first_out_up = temp_fo;
	g->head_up = temp_head;
	g->weight_up = temp_weight;

	counter = 0;
	temp_fo.clear();
	temp_head.clear();
	temp_weight.clear();

	for (unsigned i = 0; i < g->first_out.size() - 1; ++i) {
		temp_fo.push_back(counter);
		for (unsigned out_arc = g->first_out[i]; out_arc < g->first_out[i + 1]; ++out_arc) {
			unsigned y = g->head[out_arc];
			if (!changed_down[out_arc]) {
				temp_head.push_back(y);
				temp_weight.push_back(g->weight_down[out_arc]);
				counter++;
			}
		}
	}

	temp_fo.push_back(counter);
	g->first_out_down = temp_fo;
	g->head_down = temp_head;
	g->weight_down = temp_weight;

	cout << "          Up-size after after perfect customization: " << g->head_up.size() << "\n";

	cout << "          Down-size after after perfect customization: " << g->head_down.size() << "\n";
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
