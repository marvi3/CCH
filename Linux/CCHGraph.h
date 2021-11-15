/*
* CCHGraph.h
*
*  Created on: 17.12.2018
*      Author: Agent M
*/

#ifndef CCHGRAPH_H_
#define CCHGRAPH_H_

#include "vector_io.h"
#include "id_queue.h"

namespace std {

	class CCHGraph {
	public:
		CCHGraph(string directory, bool temp_travel);
		CCHGraph(bool temp_travel);
		CCHGraph();
		virtual ~CCHGraph();

                unsigned getWeight(unsigned x, unsigned y, bool direction);
                unsigned getWeight(unsigned x, unsigned y);
                void setWeight(unsigned x, unsigned y, unsigned value, bool direction);

		vector<unsigned> first_out;
		vector<unsigned> head;
		vector<unsigned> weight;

		//after perfect customization we need to manage up and down graphs separately
		vector<unsigned> first_out_up;
		vector<unsigned> head_up;
		vector<unsigned> weight_up;

		vector<unsigned> first_out_down;
		vector<unsigned> head_down;
		vector<unsigned> weight_down;

		vector<unsigned> order;
		vector<unsigned> backward_order;

		vector <unsigned> latitude;
		vector <unsigned> longitude;

		bool travel_time;

		unsigned number_nodes = first_out.size();
		unsigned number_edges = head.size();

	private:
		void initiallize(string directory);
	};

} /* namespace std */

#endif /* CCHGRAPH_H_ */
