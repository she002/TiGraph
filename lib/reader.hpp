#ifndef __READER_HPP__
#define __READER_HPP__

#include <fstream>
#include "reader.h"
#include <iostream>


void parseFile(const char* graphFile, void* (*func)(IdType, IdType, reference_data*), reference_data* data) {
	ifstream inFile;
	inFile.open(graphFile);
	if(!inFile)
	{
		cout<<"please type the correct input file name"<<endl;
		exit(1);
	}
	string tmp_string;
	long sorc, dst;
	while(!inFile.eof())
	{
		getline(inFile, tmp_string, '\n');
		//cout<<tmp_string<<endl;
		if (tmp_string[0] >= '0' && tmp_string[0] <= '9') {
				sscanf(tmp_string.c_str(), "%ld %ld", &sorc, &dst);
				//fprintf(stderr, "calling func with sorc: %ld and dst: %ld\n", sorc, dst);
				if(sorc != dst) {
       				func(sorc, dst, data);
					//cout<<"sorc: "<<sorc<<"   dst:"<<dst<<endl;
       			}
		}
	}
	inFile.close();
	
}


void* constructGraph(IdType src, IdType dst, reference_data* data) {

	//find biggest id number, to decide how many possible vertex that graphic can have
	if(src > data->maxId || dst > data->maxId)
	{
		if(src > dst)
		{
			data->maxId = src;
		}
		else
		{
			data->maxId = dst;
		}
	}
	
	data->source.push_back(src);
	data->dest.push_back(dst);
	return data;
}

#endif
