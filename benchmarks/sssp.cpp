/*
 * sssp.cpp

 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#include "../lib/engine.hpp"
#include "../lib/vertexprogram.hpp"

#include <iostream>
#include <cstdio>
#include <cmath>

#define MAXDIST (1000)
#define MAXWEIGHT (10)

typedef struct gType {
  int dist;
  gType(int d) { dist = d; }
  gType& operator+=(const gType& rhs) { 
    this->dist = std::min(this->dist, rhs.dist);
    return *this; 
  }
} GType;

typedef int VertexType;
typedef int EdgeType;
typedef GType GatherType;

IdType source = 0;
std::ofstream oFile;

class SSSPProgram: public VertexProgram<VertexType, EdgeType, GatherType> {
  bool changed; 

  GatherType initGather() {
    return MAXDIST;
  }

  EdgeDirection gatherDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0)
      return NONE;
    return IN; 
  } 

  GatherType gather(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) { 
    return (edge.Source().data() + edge.data()); 
  } 

  void apply(EngineVertex<VertexType, EdgeType>& vertex, GatherType& gValue, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0) {
      if(vertex.id() == source) 
        vertex.setData(0);
      else
        vertex.setData(MAXDIST);
    }
    else {
      changed = (gValue.dist < vertex.data());
      if(changed)
        vertex.setData(gValue.dist);
    }
  } 

  EdgeDirection scatterDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) { 
    if(engineContext.iteration() != 0) 
      return changed ? OUT : NONE; 
    return OUT;
  } 

  void scatter(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0) {
      edge.setData(1 + (vertex.id() + edge.Dest().id()) % MAXWEIGHT);
      if(vertex.id() == source)
        engineContext.scheduler().add_task(edge.Dest().id());
    }
    else
      engineContext.scheduler().add_task(edge.Dest().id());
  } 
};

class WriterProgram: public VertexProgram<VertexType, EdgeType, GatherType> {
  GatherType initGather() {
  }
  
  EdgeDirection gatherDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) {
    return NONE;
  }

  GatherType gather(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) {
  }

  void apply(EngineVertex<VertexType, EdgeType>& vertex, GatherType& gValue, EngineContext& engineContext) {
    oFile << vertex.id() << "\t" << vertex.data() << std::endl;
  }

  EdgeDirection scatterDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) {
    return NONE;
  }

  void scatter(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) {
  }
};

int main(int argc, char* argv[]) {
  initUtils(&argc, &argv);
  char graphFile[100];
  assert(parse(&argc, argv, "--sssp-graphfile=", graphFile));
  fprintf(stderr, "graphFile = %s\n", graphFile);

  int nIters = 0;
  assert(parse(&argc, argv, "--sssp-niters=", &nIters));
  fprintf(stderr, "nIters = %d\n", nIters);

  char outputFile[100];
  assert(parse(&argc, argv, "--sssp-outputfile=", outputFile));
  fprintf(stderr, "outputFile = %s\n", outputFile);

  assert(parse(&argc, argv, "--sssp-source=", &source));
  fprintf(stderr, "source = %lu\n", source);

  char concurrencyType[100];
  assert(parse(&argc, argv, "--pagerank-dynamic-tasks-allocation=", concurrencyType));
  fprintf(stderr, "dynamic concurrency = %s\n", concurrencyType);

  bool if_dynamic = false;

  if(strcmp(concurrencyType, "yes") == 0) 
  {
    if_dynamic = true;
  }
  else
  {
	if_dynamic = false;
  } 

  Engine<VertexType, EdgeType, GatherType> engine;
  engine.init(graphFile, &argc, &argv, nThreads, if_dynamic);

  double executeTime = -getTimer();
  engine.engineContext().scheduler().add_task_to_all();
  SSSPProgram ssspProgram;
  engine.run(ssspProgram, nIters, false);
  executeTime += getTimer();
  fprintf(stderr, "Processing completed in %.3lf ms\n", executeTime);

  fprintf(stderr, "Writing sssps to disk\n");
  oFile.open(outputFile);
  engine.engineContext().scheduler().add_task_to_all();

  WriterProgram writerProgram;
  engine.run(writerProgram, 1, false);
  oFile.close();

  engine.destroy();
}

