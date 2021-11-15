#include "unionFind.h"
#include "vector_io.h"
#include "id_queue.h"
#include "stdafx.h"


unsigned unionFind::unite(unsigned x_element, unsigned y_element) {
	unsigned new_element = min(x_element, y_element);
	unsigned old_element = max(x_element, y_element);
	unsigned new_element_size = set[new_element].size();
	unsigned old_element_size = set[old_element].size();

	set[new_element].resize(new_element_size + old_element_size);

	for (unsigned i = 0; i < old_element_size; i++) {
		set[new_element].push_back(set[old_element][i]);
		repr[set[old_element][i]] = new_element;
	}
	return new_element;
}

unsigned unionFind::find(unsigned x_element) {
	return repr[x_element];
}

unionFind::unionFind(vector<unsigned> nodes) {
	repr.resize(nodes.size());
	set.resize(nodes.size());
	for (unsigned i = 0; i < nodes.size(); i++) {
		repr[i] = i;
		set[i].push_back(i);
	}
}


unionFind::~unionFind() {

}
