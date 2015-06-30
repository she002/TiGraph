/*
 * engine.h
 *
 *  Created on: Apr 12, 2015
 *      Author: keval
 */

#ifndef __READER_H__
#define __READER_H__
#include "utils.h"
#include <vector>
using namespace std;


class reference_data
{
	public:
	vector<IdType>source;
	vector<IdType>dest;
	IdType maxId = 0;
};

//extern reference_data data;
void parseFile(const char* graphFile, void* (*func)(IdType, IdType, reference_data*), reference_data* data);
void* constructGraph(IdType src, IdType dst, reference_data* data);

#endif // __READER_H__
