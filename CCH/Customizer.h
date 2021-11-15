#include <vector>
#include "CCHGraph.h"

#ifndef CUSTOMIZER_H_
#define CUSTOMIZER_H_

using namespace std;

class Customizer {
    private:
		bool triangles_initialized = false;
        vector<unsigned> n_up_first;
        vector<unsigned> n_up_head;
        vector<unsigned> n_down_first;
        vector<unsigned> n_down_head;

        vector<vector<pair<unsigned, unsigned>>> n_down;
        vector<vector<pair<unsigned, unsigned>>> n_up;
		vector<vector<unsigned>> tri_down;
		vector<vector<unsigned>> tri_up;
		vector<vector<unsigned>> tri_mid;

		unsigned find_weight(unsigned, unsigned, CCHGraph*, vector<map<unsigned,unsigned>>&);
        
        void initMetric(CCHGraph *g, vector<map<unsigned,unsigned>> &w);

        vector<unsigned> intersection(vector<unsigned> &v1, vector<unsigned> &v2);
		void applyCustomization(CCHGraph *g);
        void applyBasicCustomization(CCHGraph *g);
		void applyBasicCustomizationTri(CCHGraph *g);
	    void applyPerfectCustomization(CCHGraph *g);
		void applyPerfectCustomizationTri(CCHGraph *g);
        
    public:
        Customizer() {
        
        };
        void customize(CCHGraph *g, vector<map<unsigned, unsigned>> &w);
		void initTriangles(CCHGraph *g);
		void initTrianglesCompletely(CCHGraph *g);
};

#endif /* CUSTOMIZER_H_ */
