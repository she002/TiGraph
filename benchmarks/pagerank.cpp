/*
 * pagerank.cpp

 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#include "../lib/engine.hpp"
#include "../lib/vertexprogram.hpp"
#include "../lib/utils.h"
#include <iostream>
#include <cstdio>
#include <cmath>


#define DAMPING_FACTOR 0.85
#define TOLERANCE 1.0e-2

typedef float VertexType;
typedef Empty EdgeType;
typedef float GatherType;

std::ofstream oFile;

class PageRankProgram: public VertexProgram<VertexType, EdgeType, GatherType> {
  double difference; 

  GatherType initGather() {
    return 0.0;
  }

  EdgeDirection gatherDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0)
      return NONE;
    return IN; 
  } 

  GatherType gather(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) { 
    return (edge.Source().data() / edge.Source().numOutEdges()); 
  } 

  void apply(EngineVertex<VertexType, EdgeType>& vertex, GatherType& gValue, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0) {
      double pr = 1.0;
      vertex.setData(pr);
      engineContext.scheduler().add_task(vertex.id());
    }
    else {
      double pr = (1.0 - DAMPING_FACTOR) + DAMPING_FACTOR * gValue; 
      difference = fabs(pr - vertex.data()); 
      vertex.setData(pr);
    }
  } 

  EdgeDirection scatterDirection(EngineVertex<VertexType, EdgeType>& vertex, EngineContext& engineContext) { 
    if(engineContext.iteration() == 0) 
      return NONE;
    return difference > TOLERANCE ? OUT : NONE; 
  } 

  void scatter(EngineVertex<VertexType, EdgeType>& vertex, EngineEdge<VertexType, EdgeType>& edge, EngineContext& engineContext) { 
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
    oFile << vertex.id() << "\t" << ((int) (vertex.data() * 100)) / 100.0 << std::endl;
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
  assert(parse(&argc, argv, "--pagerank-graphfile=", graphFile));
  fprintf(stderr, "graphFile = %s\n", graphFile);

  int nIters = 0;
  assert(parse(&argc, argv, "--pagerank-niters=", &nIters));
  fprintf(stderr, "nIters = %d\n", nIters);

  char outputFile[100];
  assert(parse(&argc, argv, "--pagerank-outputfile=", outputFile));
  fprintf(stderr, "outputFile = %s\n", outputFile);

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

  fprintf(stderr, "TOLERANCE = %.3lf\n", TOLERANCE);

  Engine<VertexType, EdgeType, GatherType> engine;
  engine.init(graphFile, &argc, &argv, nThreads, if_dynamic);

  double executeTime = -getTimer();
  engine.engineContext().scheduler().add_task_to_all();
  PageRankProgram pageRankProgram;
  engine.run(pageRankProgram, nIters, true);
  executeTime += getTimer();
  fprintf(stderr, "Processing completed in %.3lf ms\n", executeTime);

  fprintf(stderr, "Writing pageranks to disk\n");
  oFile.open(outputFile);
  engine.engineContext().scheduler().add_task_to_all();

  WriterProgram writerProgram;
  engine.run(writerProgram, 1, false);
  oFile.close();

  engine.destroy();
}

