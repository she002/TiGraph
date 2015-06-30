/*
 * engineedge.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __ENGINE_EDGE_H__
#define __ENGINE_EDGE_H__

#include "CSR.hpp"

enum EdgeDirection {
		IN,
		OUT,
		BOTH,
		NONE
};

template<typename VertexType, typename EdgeType> 
class EngineVertex;         // Forward declaration to break circular dependency 

/*** add the other class call "EngineEdge" which is called pagerank.cpp***/
template<typename VertexType, typename EdgeType>
class EngineEdge
{
	public:
		EngineEdge();
		EngineEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData);
		void init(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData);
		EngineVertex<VertexType, EdgeType>& Source();
		EngineVertex<VertexType, EdgeType>& Dest();
		void setData(EdgeType edgeValue);
		EdgeType data();
	private:
		EngineVertex<VertexType, EdgeType>* source;
		EngineVertex<VertexType, EdgeType>* dest;
		CSR<VertexType, EdgeType>* GraphData;

};

/*
template<typename VertexType, typename EdgeType>
class EngineInEdge : EngineEdge {
  public:
    EngineInEdge();
    EngineVertex<VertexType, EdgeType>& source();
    EdgeType data();

	
};

template<typename VertexType, typename EdgeType>
class EngineOutEdge : EngineEdge {
  public:
    EngineOutEdge();
    EngineVertex<VertexType, EdgeType>& dest();
    EdgeType data();


};

*/


#endif /* __ENGINE_EDGE_H__ */
