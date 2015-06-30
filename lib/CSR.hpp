#ifndef __CSR_HPP__
#define __CSR_HPP__

#include "CSR.h"
#include <cassert>
#include <iostream>

using namespace std;

/*
template<typename VertexType, typename EdgeType>
CSR<VertexType, EdgeType>::CSR()
{
	NumEdges = 0;
	MaxId = 0;
	InEdgeIdxs = nullptr;
	OutEdgeIdxs = nullptr;
	VertexValues = nullptr;
	SrcIndex = nullptr;
	DestIndex = nullptr;
	InEdgeValues = nullptr;
	OutEdgeValues = nullptr;
	
	
}
*/
template<typename VertexType, typename EdgeType>
CSR<VertexType, EdgeType>::~CSR()
{
	delete [] InEdgeIdxs;
	delete [] OutEdgeIdxs;
	delete [] VertexValues;
	delete [] SrcIndex;
	delete [] DestIndex;
	delete [] InEdgeValues;
	delete [] OutEdgeValues;

}

template<typename VertexType, typename EdgeType>
CSR<VertexType, EdgeType>::CSR(reference_data* data)
{
	assert(data->source.size() == data->dest.size());
	NumEdges = data->source.size();
	MaxId =  data->maxId;

	InEdgeIdxs = new IdType[MaxId + 2];
	OutEdgeIdxs = new IdType[MaxId + 2];
	VertexValues = new VertexType[MaxId + 1];
	SrcIndex = new IdType[NumEdges];
	DestIndex = new IdType[NumEdges];
	InEdgeValues = new EdgeType[NumEdges];
	OutEdgeValues = new EdgeType[NumEdges];
	
	//get num of IN edges for each vertex
	int InEdge[MaxId + 1] ;
	int OutEdge[MaxId + 1];
	
	for(int i = 0; i < MaxId + 1; i++)
	{
		InEdge[i] = 0;
		OutEdge[i] = 0;
	}
	//get Outedges and Inedges for each vertex
	vector<vector<IdType>> OutEdgesDest;
	vector<vector<IdType>> InEdgesSource;

	//IdType tmpID;
	//int counter = 0;
	for(int i = 0; i < NumEdges; i++)
	{
		//reset SrcIndex to 0
		SrcIndex[i] = 0;
		//get number of In & Out edges for each vertex and stored in InEdge & OutEdge array
		if((data->source[i] > MaxId) || (data->source[i] < 0)
			|| (data->dest[i] < 0) || (data->dest[i] > MaxId))
		{
			cerr<<"Invalid Id detected.."<<endl;
			exit(1);
		}
		InEdge[data->dest[i]]++;
		OutEdge[data->source[i]]++;
	}
	
	//insert values to InEdgeIdxs & OutEdgeIdxs Array 
	IdType SumUpInEdges = 0;
	IdType SumUpOutEdges = 0;
	for(int i = 0; i < MaxId+1; i++)
	{
		InEdgeIdxs[i] = SumUpInEdges;
		OutEdgeIdxs[i] = SumUpOutEdges;
		SumUpInEdges += InEdge[i];
		SumUpOutEdges += OutEdge[i];
		//initialize the size of OutEdgesDest and InEdgesSource
		//cout<<"in: "<<SumUpInEdges<<" out: "<<SumUpOutEdges<<endl;
		vector<IdType>Dest;
		vector<IdType>Source;
		OutEdgesDest.push_back(Source);
		InEdgesSource.push_back(Dest);
	}
	InEdgeIdxs[MaxId+1] = SumUpInEdges;
	OutEdgeIdxs[MaxId+1] = SumUpOutEdges;

	//setup OutEdgesDest and InEdgesSource
	for(int i = 0; i < NumEdges; i++)
	{
		OutEdgesDest[data->source[i]].push_back(data->dest[i]);
		InEdgesSource[data->dest[i]].push_back(data->source[i]);
	}

	//setup SrcIndex & DestIndex
	int InCounter = 0;
	int OutCounter = 0;
	for(int i = 0; i < MaxId+1; i++)
	{
		if((OutEdgesDest[i].size() != (OutEdgeIdxs[i+1] - OutEdgeIdxs[i])) ||
			(InEdgesSource[i].size() != (InEdgeIdxs[i+1] - InEdgeIdxs[i])))
		{
			/*
			cerr<<OutEdgesDest[i].size()<<endl;
			cerr<<(OutEdgeIdxs[i+1] - OutEdgeIdxs[i])<<endl;
			cerr<<InEdgesSource[i].size()<<endl;
			cerr<<(InEdgeIdxs[i+1] - InEdgeIdxs[i])<<endl;
			*/
			cerr<<i<<" Wrong set up on InEdgeIdx or OutEdgeIdx.."<<endl;
			exit(1);
		}
		//SrcIndex
		for(int j = 0; j < InEdgesSource[i].size(); j++)
		{
			if(InCounter > NumEdges){ cerr<<"Something wrong with InEdgesSrouce vector"<<endl;}
			SrcIndex[InCounter] = InEdgesSource[i][j];
			InCounter++;
		}
		//DestIndex
		for(int j = 0; j < OutEdgesDest[i].size(); j++)
		{
			if(OutCounter > NumEdges){ cerr<<"Something wrong with OutEdgesDest vector"<<endl;}
			DestIndex[OutCounter] = OutEdgesDest[i][j];
			OutCounter++;
		}	
		
	}
	/*
	cout<<"EdgesIdx: \n";
	for(int i = 0; i < MaxId + 2; i++)
	{
		cout<<"In: "<<InEdgeIdxs[i]<<"	Out:"<<OutEdgeIdxs[i]<<endl;
	}
	cout<<"Src&Edge Index: \n";
	for(int i = 0; i < NumEdges; i++)
	{
		cout<<"Src: "<<SrcIndex[i]<<"	Dest:"<<DestIndex[i]<<endl;
	}
	*/
}
template<typename VertexType, typename EdgeType>
VertexType CSR<VertexType, EdgeType>::getVertexValue(IdType Id)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
	}
	return VertexValues[Id];
}

template<typename VertexType, typename EdgeType>
long int CSR<VertexType, EdgeType>::getInEdgeSrcId(IdType Id, int i)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = InEdgeIdxs[Id];
	int nextStartPos = InEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th In edge for this vertex"<<endl;
	}
	else
	{
		IdType SrcId = SrcIndex[startPos + i];
		return (long int)SrcId;
	}
	return -1;
}

template<typename VertexType, typename EdgeType>
long int CSR<VertexType, EdgeType>::getOutEdgeDestId(IdType Id, int i)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = OutEdgeIdxs[Id];
	int nextStartPos = OutEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th In edge for this vertex"<<endl;
	}
	else
	{
		IdType DestId = DestIndex[startPos + i];
		return (long int)DestId;
	}
	return -1;
}

template<typename VertexType, typename EdgeType>
EdgeType CSR<VertexType, EdgeType>::getInEdgeValue(IdType Id, int i)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = InEdgeIdxs[Id];
	int nextStartPos = InEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th In edge for this vertex"<<endl;
	}
	else
	{
		return InEdgeValues[startPos + i];
	}
	return 0;
}

template<typename VertexType, typename EdgeType>
EdgeType CSR<VertexType, EdgeType>::getOutEdgeValue(IdType Id, int i)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = OutEdgeIdxs[Id];
	int nextStartPos = OutEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th Out edge for this vertex"<<endl;
	}
	else
	{
		return OutEdgeValues[startPos + i];
		
	}
	return 0;
}

template<typename VertexType, typename EdgeType>
int CSR<VertexType, EdgeType>::getNumInEdges(IdType Id)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = InEdgeIdxs[Id];
	int nextStartPos = InEdgeIdxs[Id + 1];
	return (nextStartPos - startPos);
}

template<typename VertexType, typename EdgeType>
int CSR<VertexType, EdgeType>::getNumOutEdges(IdType Id)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = OutEdgeIdxs[Id];
	int nextStartPos = OutEdgeIdxs[Id + 1];
	return (nextStartPos - startPos);
}

template<typename VertexType, typename EdgeType>
void CSR<VertexType, EdgeType>::setVertexValue(IdType Id, VertexType value)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
	}
	VertexValues[Id] = value;
}

template<typename VertexType, typename EdgeType>
void CSR<VertexType, EdgeType>::setInEdgeValue(IdType Id, int i, EdgeType value)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = InEdgeIdxs[Id];
	int nextStartPos = InEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th In edge for this vertex"<<endl;
	}
	else
	{
		InEdgeValues[startPos + i] = value;
	}
}

template<typename VertexType, typename EdgeType>
void CSR<VertexType, EdgeType>::setOutEdgeValue(IdType Id, int i, EdgeType value)
{
	if(Id > MaxId || Id < 0)
	{
		cerr<<"no such Id"<<endl;
		exit(0);
	}
	int startPos = OutEdgeIdxs[Id];
	int nextStartPos = OutEdgeIdxs[Id + 1];
	if((nextStartPos - startPos) == 0)
	{
		cerr<<"no In Edge for this Id"<<endl;
	}
	else if(i >= (nextStartPos - startPos))
	{
		cerr<<"there is no (i)th In edge for this vertex"<<endl;
	}
	else
	{
		OutEdgeValues[startPos + i] = value;
	}
}
template<typename VertexType, typename EdgeType>
int CSR<VertexType, EdgeType>::getMaxId()
{
	return MaxId;
}

template<typename VertexType, typename EdgeType>
int CSR<VertexType, EdgeType>::getNumEdges()
{
	return NumEdges;
}

#endif
