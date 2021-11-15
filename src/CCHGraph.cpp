/*
 * CCHGraph.cpp
 *
 *  Created on: 17.12.2018
 *      Author: Agent M
 */

#include "CCHGraph.h"
#include "vector_io.h"
#include "id_queue.h"

namespace std {

void CCHGraph::initiallize(string temp_directory) {

	first_out = load_vector<unsigned>(temp_directory + "first_out");
	head = load_vector<unsigned>(temp_directory + "head");

	latitude = load_vector<unsigned>(temp_directory + "latitude");
	longitude = load_vector<unsigned>(temp_directory + "longitude");

	number_nodes = first_out.size() - 1;
	number_edges = head.size();

	if(travel_time) {
		order = load_vector<unsigned>(temp_directory + "travel_time_ch\\order");
	} else {
		order = load_vector<unsigned>(temp_directory + "geo_distance_ch\\order");
	}
}

CCHGraph::CCHGraph(string directory, bool temp_travel) {
	//constructor for existing graphs
	travel_time = temp_travel;
	initiallize(directory);
}

CCHGraph::CCHGraph(bool temp_travel) {
	//constructor for empty graphs
	travel_time = temp_travel;

}

CCHGraph::~CCHGraph() {
	//destructor
}


} /* namespace std */
