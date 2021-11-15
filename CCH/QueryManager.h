#include <vector>
#include "CCHGraph.h"

#ifndef QUERYMANAGER_H_
#define QUERYMANAGER_H_

using namespace std;

class QueryManager {
    private:
        
        CCHGraph g;
        vector<unsigned> d_up;
        vector<unsigned> d_down;

        vector<unsigned> predecessor_up;
        vector<unsigned> predecessor_down;

        vector<unsigned> unpacked_path;

        vector<unsigned> path_up;
        vector<unsigned> path_down;

        unsigned last_collision_node;
        unsigned last_start_node;
        unsigned last_target_node;

        unsigned dijkstra_query_count = 0;
        vector<unsigned> d_forward;
        vector<unsigned> d_backward;
        vector<unsigned> dijkstra_closed_nodes_forward;
        vector<unsigned> dijkstra_closed_nodes_backward;

        void init();
        void initParents();
        unsigned getWeight(unsigned, unsigned, bool);
        vector<unsigned> insertNode(vector<unsigned>&, unsigned, unsigned);
        
    public:
        vector<unsigned> parent_up;     //made public for testing purposes
        vector<unsigned> parent_down;
        vector<unsigned> unpack_path();
        vector<unsigned> visited_nodes;
        vector<unsigned> visited_edges;

        QueryManager(CCHGraph &_g): g(_g) {init(); initParents();};
        QueryManager();
        
        unsigned eliminationTreeQuery(unsigned s, unsigned t);
        unsigned basicQuery(unsigned s, unsigned t);
    
};

#endif /* QUERYMANAGER_H_ */
