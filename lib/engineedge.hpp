#ifndef __ENGINE_EDGE_HPP__
#define __ENGINE_EDGE_HPP__

#include "engineedge.h"

template<typename VertexType, typename EdgeType>
EngineEdge<VertexType, EdgeType>::EngineEdge():source(nullptr), dest(nullptr), GraphData(nullptr){}

template<typename VertexType, typename EdgeType>
EngineEdge<VertexType, EdgeType>::EngineEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData)
:source(Src), dest(Dst), GraphData(CSRData){}

template<typename VertexType, typename EdgeType>
void EngineEdge<VertexType, EdgeType>::init(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData)
{
	source = Src;
	dest = Dst;
	GraphData = CSRData;
	//cerr<<"Under engineedge.hpp,Inside init(): \n"<<"src "<<&(*source)<<" dest "<<&(*dest)<<endl;
}
template<typename VertexType, typename EdgeType>
EngineVertex<VertexType, EdgeType>& EngineEdge<VertexType, EdgeType>::Source()
{
	if(source)
	{
		return *source;
	}
	else
	{
		cerr<<"Error when calling EngineVertex<VertexType, EdgeType>& EngineEdge<Vertex, EdgeType>::Source():	the EngineEdge has a variable that is a nullpointer"<<endl;
		exit(1);
	}
}

template<typename VertexType, typename EdgeType>
EngineVertex<VertexType, EdgeType>& EngineEdge<VertexType, EdgeType>::Dest()
{
	if(dest)
	{
		return *dest;
	}
	else
	{
		cerr<<"Error when calling EngineVertex<VertexType, EdgeType>& EngineEdge<Vertex, EdgeType>::dest():	the EngineEdge has a variable that is a nullpointer"<<endl;
		exit(1);
	}
}

template<typename VertexType, typename EdgeType>
void EngineEdge<VertexType, EdgeType>::setData(EdgeType edgeValue)
{
	IdType SourceId = source->id();
	IdType DestId = dest->id();
	int Num_OutEdges = source->numOutEdges();
	//bool found = false;
	for(int i = 0; i < Num_OutEdges; i++)
	{
		if(GraphData->getOutEdgeDestId(SourceId, i) == DestId)
		{
			//found = true;
			GraphData->setOutEdgeValue(SourceId, i, edgeValue);
			return;
		}
	}
	cerr<<"cant find edge value, this engineedge variable may contained wrong data"<<endl;
	exit(1);
}

template<typename VertexType, typename EdgeType>
EdgeType EngineEdge<VertexType, EdgeType>::data()
{
	IdType SourceId = source->id();
	IdType DestId = dest->id();
	int Num_OutEdges = source->numOutEdges();
	//bool found = false;
	for(int i = 0; i < Num_OutEdges; i++)
	{
		if(GraphData->getOutEdgeDestId(SourceId, i) == DestId)
		{
			//found = true;
			return GraphData->getOutEdgeValue(SourceId, i);
		}
	}
	cerr<<"cant find edge value, this engineedge variable may contained wrong data"<<endl;
	exit(1);
}


#endif
