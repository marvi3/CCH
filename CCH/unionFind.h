/*
* CCHGraph.h
*
*  Created on: 11.03.2019
*      Author: Agent M
*/

#include "vector_io.h"
#include "id_queue.h"
#pragma once

class unionFind
{
public:
	unionFind(vector<unsigned> nodes);
	~unionFind();
	
private:
	unsigned unite(unsigned x_element, unsigned y_element);
	unsigned find(unsigned x_element);
	vector<vector<unsigned>> set;
	vector<unsigned> repr;

};

