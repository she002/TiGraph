/*
 * enginevertex.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __ENGINE_VERTEX_H__
#define __ENGINE_VERTEX_H__

#include "engineedge.hpp"


template<typename VertexType, typename EdgeType>   
class EngineVertex {

  public:
    EngineVertex();
	EngineVertex(IdType theId, CSR<VertexType, EdgeType>* CSRData); //Added Constructor
	void init(IdType theId, CSR<VertexType, EdgeType>* CSRData);    
	IdType id();
    VertexType data();
    void setData(VertexType value);

    int numInEdges();
    int numOutEdges();
    EngineEdge<VertexType, EdgeType>& inEdge(int i);
    EngineEdge<VertexType, EdgeType>& outEdge(int i);

	void putInEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData);
	void putOutEdge(EngineVertex<VertexType, EdgeType>* Src, EngineVertex<VertexType, EdgeType>* Dst, CSR<VertexType, EdgeType>* CSRData);
	~EngineVertex();
  private:
	IdType VertexId;
	CSR<VertexType, EdgeType>* GraphData;
	EngineEdge<VertexType, EdgeType>* InEdgeList;
	EngineEdge<VertexType, EdgeType>* OutEdgeList;
	int num_InEdges;
	int num_OutEdges;
	//VertexType data;

};

#endif /* __ENGINE_VERTEX_H__ */
