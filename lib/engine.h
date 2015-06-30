/*
 * engine.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "vertexprogram.hpp"
#include "enginecontext.hpp"
#include "args.h"
#include <pthread.h>


template<typename VertexType, typename EdgeType, typename GatherType>
class Engine {

  public:
    Engine();
    void init(const char* graphFile, int *argc, char ***argv, unsigned int num_threads = 1, bool if_dynamic = false);
    void destroy();
    void run(VertexProgram<VertexType, EdgeType, GatherType>& vertexProgram, int nIters = -1, bool parallel = false);
	void print();
	EngineContext& engineContext();
	void* thread_worker(void* arg);
	
	static void* thread_worker_helper(void* arg);
  private:
	
	CSR<VertexType, EdgeType>* GraphData;
	EngineContext* EngineInfo;
	EngineVertex<VertexType, EdgeType>* VerticesList;
	pthread_t* ThreadList;
	pthread_barrier_t* EngineBarrier;
	pthread_mutex_t EngineMutex;
	bool DieFlag;
	IdType NumThreads;
	VertexProgram<VertexType, EdgeType, GatherType>* vertex_Program;
	args<VertexType, EdgeType, GatherType>* ArgumentList;
	pthread_rwlock_t* RwlockList;
	bool dynamic;
	IdType NewJob;
};

#endif /* __ENGINE_H__ */
