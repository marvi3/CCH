#include <vector>
#include "CCHGraph.h"

#ifndef CUSTOMIZER_H_
#define CUSTOMIZER_H_

using namespace std;

class Customizer {
    private:
        vector<unsigned> n_up_first;
        vector<unsigned> n_up_head;
        vector<unsigned> n_down_first;
        vector<unsigned> n_down_head;

        vector<vector<unsigned>> n_down;
        vector<vector<unsigned>> n_up;

		unsigned find_weight(unsigned, unsigned, CCHGraph*, vector<map<unsigned,unsigned>>&);
        
        void initMetric(CCHGraph *g, vector<map<unsigned,unsigned>> &w);
        void initTriangles(CCHGraph *g);
	void copyVector(vector<unsigned> *v1, vector<unsigned> *v2);

        vector<unsigned> intersection(vector<unsigned> &v1, vector<unsigned> &v2);
        void applyBasicCustomization(CCHGraph *g);
        void applyBasicCustomization_old(CCHGraph *g);
	    void applyPerfectCustomization(CCHGraph *g);
        void applyPerfectCustomization_old(CCHGraph *g);
        
    public:
        Customizer() {
        
        };
        void customize(CCHGraph *g, vector<map<unsigned, unsigned>> &w);
};

#endif /* CUSTOMIZER_H_ */
