/*
 * vertexprogram.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __VERTEX_PROGRAM_HPP__
#define __VERTEX_PROGRAM_HPP__

#include "enginevertex.hpp"
#include "enginecontext.hpp"


template<typename VertexType, typename EdgeType, typename GatherType>
class VertexProgram {
  public:
	
    virtual ~VertexProgram() {
    }

    virtual GatherType initGather() {
      fprintf(stderr, "Base initGather called.\n");
      assert(false);
    }

    virtual EdgeDirection gatherDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) {
      fprintf(stderr, "Base gatherDirection called.\n");
      assert(false);
    }

    virtual GatherType gather(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) {
      fprintf(stderr, "Base gather called.\n");
      assert(false);
    }

    virtual void apply(EngineVertex<VertexType, EdgeType>& vertex, GatherType& gValue, EngineContext& engineContext) {
      fprintf(stderr, "Base apply called.\n");
      assert(false);
    }

    virtual EdgeDirection scatterDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) {
      fprintf(stderr, "Base scatterDirection called.\n");
      assert(false);
    }

    virtual void scatter(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) {
      fprintf(stderr, "Base scatter called.\n");
      assert(false);
    }

};

#endif /* __VERTEX_PROGRAM_HPP__ */
