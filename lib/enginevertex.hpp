#ifndef __ENGINE_VERTEX_HPP__
#define __ENGINE_VERTEX_HPP__

#include "enginevertex.h"

template<typename VertexType, typename EdgeType>  
EngineVertex<VertexType, EdgeType>::EngineVertex():VertexId(),GraphData(nullptr),InEdgeList(nullptr),OutEdgeList(nullptr), num_InEdges(0), num_OutEdges(0) {}

template<typename VertexType, typename EdgeType> 
EngineVertex<VertexType, EdgeType>::EngineVertex(IdType theId, CSR<VertexType, EdgeType>* CSRData)
{
	VertexId = theId;
	GraphData = CSRData;
	int num_allInEdges = GraphData->getNumInEdges(VertexId);
	int num_allOutEdges = GraphData->getNumOutEdges(VertexId);
	InEdgeList = new EngineEdge<VertexType, EdgeType> [num_allInEdges];
	OutEdgeList = new EngineEdge<VertexType, EdgeType> [num_allOutEdges];
	num_InEdges = 0;
	num_OutEdges = 0;
}

template<typename VertexType, typename EdgeType> 
void EngineVertex<VertexType, EdgeType>::init(IdType theId, CSR<VertexType, EdgeType>* CSRData)
{
	VertexId = theId;
	GraphData = CSRData;
	int num_allInEdges = GraphData->getNumInEdges(VertexId);
	int num_allOutEdges = GraphData->getNumOutEdges(VertexId);
	InEdgeList = new EngineEdge<VertexType, EdgeType> [num_allInEdges];
	OutEdgeList = new EngineEdge<VertexType, EdgeType> [num_allOutEdges];
	//cerr<<"id "<<theId<<"\nUnder enginevertex.hpp, in init()\n InEdgeList has size "<<num_allInEdges<<" and OutEdgeList has size "<<num_allOutEdges<<endl;
	num_InEdges = 0;
	num_OutEdges = 0;
	
}
//template<typename VertexType, typename EdgeType>  
//EngineVertex<VertexType, EdgeType>::~EngineVertex(){}

template<typename VertexType, typename EdgeType>
IdType EngineVertex<VertexType, EdgeType>::id()
{
	//cerr<<"return vertexID"<<VertexId<<endl;
	return VertexId;
}

template<typename VertexType, typename EdgeType>
VertexType EngineVertex<VertexType, EdgeType>::data()
{
	return GraphData->getVertexValue(VertexId);
}

template<typename VertexType, typename EdgeType>
void EngineVertex<VertexType, EdgeType>::setData(VertexType value)
{
	GraphData->setVertexValue(VertexId, value);
}

template<typename VertexType, typename EdgeType>
int EngineVertex<VertexType, EdgeType>::numInEdges()
{
	return GraphData->getNumInEdges(VertexId);
}

template<typename VertexType, typename EdgeType>
int EngineVertex<VertexType, EdgeType>::numOutEdges()
{
	return GraphData->getNumOutEdges(VertexId);
}

template<typename VertexType, typename EdgeType>
EngineEdge<VertexType, EdgeType>& EngineVertex<VertexType, EdgeType>::inEdge(int i)
{
	
	if(numInEdges() > i)
	{
		if(InEdgeList)
		{
			return InEdgeList[i];
		}
		else
		{
			cerr<<"Error: This EnginVertex has a InEdgesList which is a null pointer"<<endl;
			exit(1);
		}
	}
	else
	{
		cerr<<"bad parameter for function EngineEdge<VertexType, EdgeType>& inEdge"<<endl;
		exit(1);
	}
}	

template<typename VertexType, typename EdgeType>
EngineEdge<VertexType, EdgeType>& EngineVertex<VertexType, EdgeType>::outEdge(int i)
{
	if(numOutEdges() > i)
	{
		if(OutEdgeList)
		{
			return OutEdgeList[i];
		}
		else
		{
			cerr<<"Error: This EnginVertex has a OutEdgesList which is a null pointer"<<endl;
			exit(1);
		}
	}
	else
	{
		cerr<<"bad parameter for function EngineEdge<VertexType, EdgeType>& OutEdge"<<endl;
		exit(1);
	}
}

template<typename VertexType, typename EdgeType>
void EngineVertex<VertexType, EdgeType>::putInEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData)
{
	if(num_InEdges < numInEdges())
	{
		//cerr<<"Under enginevertex.hpp,Inside putInEdge(): \n"<<"src "<<&(*Src)<<" dest "<<&(*Dst)<<endl;
		InEdgeList[num_InEdges].init(Src, Dst, CSRData);
		num_InEdges++;
	}
	else
	{
		cerr<<"there should not have more than "<<numInEdges()<<" In Edges in this graph"<<endl;
		exit(1);
	}
}

template<typename VertexType, typename EdgeType>
void EngineVertex<VertexType, EdgeType>::putOutEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData)
{
	if(num_OutEdges < numOutEdges())
	{
		OutEdgeList[num_OutEdges].init(Src, Dst, CSRData);
		num_OutEdges++;
	}
	else
	{
		cerr<<"there should not have more than "<<numOutEdges()<<" Out Edges in this graph"<<endl;
		exit(1);
	}
}


template<typename VertexType, typename EdgeType>
EngineVertex<VertexType, EdgeType>::~EngineVertex()
{
	//cerr<<VertexId<<" delete called"<<endl;
	delete [] InEdgeList;
	delete [] OutEdgeList;
	
}
#endif
