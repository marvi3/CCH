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

        vector<vector<unsigned>> triangel_weight;
        vector<vector<unsigned>> triangel_node;
        vector<unsigned> unpacked_path;

        vector<unsigned> path_up;
        vector<unsigned> path_down;

        unsigned last_collision_node;

        void init();
        void initParents();
        unsigned getWeight(unsigned, unsigned, bool);
        vector<unsigned> insertNode(vector<unsigned>&, unsigned, unsigned);
        
    public:
        vector<unsigned> parent_up;     //made public for testing purposes
        vector<unsigned> parent_down;
        vector<unsigned> unpack_path();
        QueryManager(CCHGraph &_g): g(_g) {init(); initParents();};
        QueryManager();
        
        unsigned eliminationTreeQuery(unsigned s, unsigned t);
    
};

#endif /* QUERYMANAGER_H_ */
