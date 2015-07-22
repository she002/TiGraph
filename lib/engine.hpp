#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "engine.h"
#include "reader.hpp"
#include <algorithm>

template<typename VertexType, typename EdgeType, typename GatherType>
Engine <VertexType, EdgeType, GatherType>::Engine()
{
	GraphData = nullptr;
	EngineInfo = nullptr;
	VerticesList = nullptr;
	ThreadList = nullptr;
	EngineBarrier = nullptr;
	ArgumentList = nullptr;
	RwlockList = nullptr;
	DieFlag = false;
	dynamic = false;
	NumThreads = 0;
	NewJob = 0;
}

template<typename VertexType, typename EdgeType, typename GatherType>
void Engine<VertexType, EdgeType, GatherType>::init(const char* graphFile, int *argc, char ***argv,unsigned int num_threads, bool if_dynamic)
{
	reference_data* myData = new reference_data;
	parseFile(graphFile, constructGraph, myData);
	GraphData = new CSR<VertexType, EdgeType>(myData);
	IdType VSize = GraphData->getMaxId() + 1;
	EngineInfo = new EngineContext(VSize);
	VerticesList = new EngineVertex<VertexType, EdgeType>[VSize];
	//create rwlock for each vertex;
	RwlockList = new pthread_rwlock_t[VSize];
	delete myData;
	for(IdType i = 0; i < VSize; i++)
	{
		VerticesList[i].init(i, GraphData);
		pthread_rwlock_init(&RwlockList[i], NULL);
	}
	for(IdType i = 0; i < VSize; i++)
	{
		
		int num_OutEdges = GraphData->getNumOutEdges(i);
		for(int j = 0; j < num_OutEdges; j++)
		{
			
			IdType destId = (IdType)(GraphData->getOutEdgeDestId(i, j));
			VerticesList[i].putOutEdge(&VerticesList[i], &VerticesList[destId], GraphData);
			
		}
		int num_InEdges = GraphData->getNumInEdges(i);
		for(int j = 0; j < num_InEdges; j++)
		{
			IdType srcId = (IdType)(GraphData->getInEdgeSrcId(i, j));	
			VerticesList[i].putInEdge(&VerticesList[srcId], &VerticesList[i], GraphData);
		}
	}
	NumThreads = num_threads;
	ThreadList = new pthread_t[NumThreads];
	EngineBarrier = new pthread_barrier_t;
	EngineMutex = PTHREAD_MUTEX_INITIALIZER;
	DieFlag = false;
	dynamic = if_dynamic;
	pthread_barrier_init(EngineBarrier, NULL, NumThreads);
	ArgumentList = new args<VertexType, EdgeType, GatherType>[NumThreads];
	for(IdType i = 1; i < NumThreads; i++)
	{
		ArgumentList[i].tid = i;
		ArgumentList[i].context = this;
		cerr<<i<<" thread created"<<endl;
		pthread_create(&ThreadList[i], NULL, &thread_worker_helper, &ArgumentList[i]);
	}
}

template<typename VertexType, typename EdgeType, typename GatherType>
void Engine<VertexType, EdgeType, GatherType>::destroy()
{
	for(IdType i = 0; i < (GraphData->getMaxId() + 1); i++)
	{
		pthread_rwlock_destroy(&RwlockList[i]);
	}
	delete [] RwlockList;
	delete GraphData;
	delete EngineInfo;
	delete [] VerticesList;
	delete [] ThreadList;
	pthread_barrier_destroy(EngineBarrier);
	delete EngineBarrier;
	delete [] ArgumentList;
}
template<typename VertexType, typename EdgeType, typename GatherType>
void Engine<VertexType, EdgeType, GatherType>::Engine_Gather(GatherType& gatherValue, EdgeDirection GatherDir, IdType CurrId)
{
	if(GatherDir == IN)
	{
		for(int j = 0; j < VerticesList[CurrId].numInEdges(); j++)
		{
			gatherValue += vertex_Program->gather(VerticesList[CurrId], VerticesList[CurrId].inEdge(j), *EngineInfo);
		}			
	}
	else if(GatherDir == OUT)
	{
		for(int j = 0; j < VerticesList[CurrId].numOutEdges(); j++)
		{
			gatherValue += vertex_Program->gather(VerticesList[CurrId], VerticesList[CurrId].outEdge(j), *EngineInfo);
		}
	}
	else if(GatherDir == BOTH)
	{
		for(int j = 0; j < VerticesList[CurrId].numInEdges(); j++)
		{
			gatherValue += vertex_Program->gather(VerticesList[CurrId], VerticesList[CurrId].inEdge(j), *EngineInfo);
		}
		for(int j = 0; j < VerticesList[CurrId].numOutEdges(); j++)
		{
			gatherValue += vertex_Program->gather(VerticesList[CurrId], VerticesList[CurrId].outEdge(j), *EngineInfo);
		}
	}
}
template<typename VertexType, typename EdgeType, typename GatherType>
void Engine<VertexType, EdgeType, GatherType>::Engine_Scatter(IdType CurrId)
{
	EdgeDirection ScatterDir = vertex_Program->scatterDirection(VerticesList[CurrId], *EngineInfo);
	if(ScatterDir == OUT)
	{
		for(int j = 0; j < VerticesList[CurrId].numOutEdges(); j++)
		{
			vertex_Program->scatter(VerticesList[CurrId], VerticesList[CurrId].outEdge(j), *EngineInfo);
		}	
	}
	else if(ScatterDir == IN)
	{
		for(int j = 0; j < VerticesList[CurrId].numInEdges(); j++)
		{
			vertex_Program->scatter(VerticesList[CurrId], VerticesList[CurrId].inEdge(j), *EngineInfo);
		}
	}
	else if(ScatterDir == BOTH)
	{
		for(int j = 0; j < VerticesList[CurrId].numOutEdges(); j++)
		{
			vertex_Program->scatter(VerticesList[CurrId], VerticesList[CurrId].outEdge(j), *EngineInfo);
		}
		for(int j = 0; j < VerticesList[CurrId].numInEdges(); j++)
		{
			vertex_Program->scatter(VerticesList[CurrId], VerticesList[CurrId].inEdge(j), *EngineInfo);
		}
	}
}
template<typename VertexType, typename EdgeType, typename GatherType>
void Engine<VertexType, EdgeType, GatherType>::run(VertexProgram<VertexType, EdgeType, GatherType>& vertexProgram, int nIters, bool parallel)
{
    vertex_Program = &vertexProgram;
	GatherType iGather = vertex_Program->initGather();
    BitsetScheduler* scheduler = &EngineInfo->scheduler();
	IdType VSize = GraphData->getMaxId()+1;
	NewJob = 0;
	IdType CurrId = 0;
	IdType EndId = 0;
	DieFlag = false;
	int n = 0;
	while(1)
	{
		scheduler->new_iteration();
		if(parallel)
		{
			pthread_barrier_wait(EngineBarrier);
			//cout<<"pass first barrier"<<endl;
			if(dynamic)
			{
				EndId = GraphData->getMaxId();
				pthread_mutex_lock(&EngineMutex);
				CurrId = NewJob;
				++NewJob;
				//cout<<"beging NewJob = "<<NewJob<<endl;
				//cout<<"1: "<<CurrId<<" "<<EndId<<endl;
				pthread_mutex_unlock(&EngineMutex);
			}
			else
			{
				CurrId = 0;
				EndId = (VSize/NumThreads)-1;
			}
		}	
		else
		{
			CurrId = 0;
			EndId = GraphData->getMaxId();
		}
		if(DieFlag)
		{
			break;
		}
		while(CurrId <= EndId)
		{
			//cout<<CurrId<<endl;
			if(scheduler->is_scheduled(CurrId))
			{
				EdgeDirection GatherDir = vertex_Program->gatherDirection(VerticesList[CurrId], *EngineInfo);
				GatherType gatherValue = iGather;

				//lock up 
				vector<IdType> tmpvector;
				if(GatherDir == IN)
				{
					int numIn = VerticesList[CurrId].numInEdges();
					IdType tmplist[numIn+1];
					for(int j = 0; j < numIn; j++)
					{
						tmplist[j] = VerticesList[CurrId].inEdge(j).Source().id();
					}
					tmplist[numIn] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numIn+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}	
				}
				else if(GatherDir == OUT)
				{
					int numOut = VerticesList[CurrId].numOutEdges();
					IdType tmplist[numOut+1];
					for(int j = 0; j < numOut; j++)
					{
						tmplist[j] = VerticesList[CurrId].outEdge(j).Dest().id();
					}
					tmplist[numOut] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numOut+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}
				}
				else if(GatherDir == BOTH)
				{
					int numIn = VerticesList[CurrId].numInEdges();
					int numOut = VerticesList[CurrId].numOutEdges();
					IdType tmplist[numIn+numOut+1];
					for(int j = 0; j < numOut; j++)
					{
						tmplist[j] = VerticesList[CurrId].outEdge(j).Dest().id();
					}
					for(int j = 0; j < numIn; j++)
					{
						tmplist[numOut+j] = VerticesList[CurrId].outEdge(j).Source().id();
					}
					tmplist[numOut+numIn] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numOut+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}
				}
							
				//get gatherValue
				Engine_Gather(gatherValue, GatherDir, CurrId);

				//unlock read lock on neighbor vertex
				if(GatherDir == IN || GatherDir == OUT || GatherDir == BOTH)
				{
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] != CurrId)
						{
							pthread_rwlock_unlock(&RwlockList[tmpvector[j]]);
						}
					}
				}
				vertex_Program->apply(VerticesList[CurrId], gatherValue, *EngineInfo);

				//unlock write lock on current vertex
				if(GatherDir == IN || GatherDir == OUT || GatherDir == BOTH)
				{
						pthread_rwlock_unlock(&RwlockList[CurrId]);
				}

				//scatter 
				
				Engine_Scatter(CurrId);
			}
			
			if(parallel)
			{
				if(dynamic)
				{
					pthread_mutex_lock(&EngineMutex);
					CurrId = NewJob;
					++NewJob;
					//cout<<"1: "<<CurrId<<endl;
					pthread_mutex_unlock(&EngineMutex);
				}
				else
				{
					++CurrId;
				}
			}
			else
			{
				++CurrId;
			}	
		}
		if(parallel)
		{
			pthread_barrier_wait(EngineBarrier);
			if(dynamic)
			{
				pthread_mutex_lock(&EngineMutex);
				NewJob = 0;
			    //cout<<"end NewJob = "<<NewJob<<endl;
				pthread_mutex_unlock(&EngineMutex);
			}
			else
			{
				CurrId = 0;
			}
		}
		EngineInfo->iterationIncrement();
		if(!scheduler->anyScheduledTasks() ||n ==(nIters-1)) 
		{
			DieFlag = true;
		}
		n++;
	}
	cout<<"processing converges in "<<EngineInfo->iteration()<<endl;
}

template<typename VertexType, typename EdgeType, typename GatherType>
EngineContext& Engine<VertexType, EdgeType, GatherType>::engineContext()
{
	return *EngineInfo;
}

template<typename VertexType, typename EdgeType, typename GatherType>
void* Engine<VertexType, EdgeType, GatherType>::thread_worker(void* arg)
{
	IdType* tid = (IdType*)(arg);
	IdType ThreadId = *tid;
	BitsetScheduler* scheduler = &EngineInfo->scheduler();
	IdType VSize = GraphData->getMaxId()+1;
	IdType NumAverage = VSize/NumThreads;
	IdType EndId, CurrId;
	(ThreadId == (NumThreads-1))? EndId = GraphData->getMaxId() : EndId = NumAverage * (ThreadId + 1) - 1;
	//if(ThreadId == 3) cout<<ThreadId<<" "<<VSize<<" "<<CurrId<<" "<<EndId<<endl;
	while(1)
	{
		pthread_barrier_wait(EngineBarrier);
		CurrId = NumAverage*(ThreadId);
        GatherType iGather = vertex_Program->initGather();
		if(dynamic)
		{
			EndId = GraphData->getMaxId();
			pthread_mutex_lock(&EngineMutex);
			CurrId = NewJob;
			++NewJob;
			//cout<<"2: "<<CurrId<<" "<<EndId<<endl;
			pthread_mutex_unlock(&EngineMutex);
		}
		if(DieFlag)
		{
			break;
		}
		while(CurrId <= EndId)
		{
			
			if(scheduler->is_scheduled(CurrId))
			{
				EdgeDirection GatherDir = vertex_Program->gatherDirection(VerticesList[CurrId], *EngineInfo);
				GatherType gatherValue = iGather;

				//lock up 
				vector<IdType> tmpvector;
				if(GatherDir == IN)
				{
					int numIn = VerticesList[CurrId].numInEdges();
					IdType tmplist[numIn+1];
					for(int j = 0; j < numIn; j++)
					{
						tmplist[j] = VerticesList[CurrId].inEdge(j).Source().id();
					}
					tmplist[numIn] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numIn+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}
					
				}
				else if(GatherDir == OUT)
				{
					int numOut = VerticesList[CurrId].numOutEdges();
					IdType tmplist[numOut+1];
					for(int j = 0; j < numOut; j++)
					{
						tmplist[j] = VerticesList[CurrId].outEdge(j).Dest().id();
					}
					tmplist[numOut] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numOut+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}
				}
				else if(GatherDir == BOTH)
				{
					int numIn = VerticesList[CurrId].numInEdges();
					int numOut = VerticesList[CurrId].numOutEdges();
					IdType tmplist[numIn+numOut+1];
					for(int j = 0; j < numOut; j++)
					{
						tmplist[j] = VerticesList[CurrId].outEdge(j).Dest().id();
					}
					for(int j = 0; j < numIn; j++)
					{
						tmplist[numOut+j] = VerticesList[CurrId].outEdge(j).Source().id();
					}
					tmplist[numOut+numIn] = CurrId;
					tmpvector = vector<IdType>(tmplist, tmplist+numOut+1);
					sort(tmpvector.begin(), tmpvector.end());
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] == CurrId)
						{
							pthread_rwlock_wrlock(&RwlockList[tmpvector[j]]);
						}
						else
						{
							pthread_rwlock_rdlock(&RwlockList[tmpvector[j]]);
						}
					}
				}

				//gather
				Engine_Gather(gatherValue, GatherDir, CurrId);

				//unlock read lock on neighbor vertex
				if(GatherDir == IN || GatherDir == OUT || GatherDir == BOTH)
				{
					for(int j = 0; j < tmpvector.size(); j++)
					{
						if(tmpvector[j] != CurrId)
						{
							pthread_rwlock_unlock(&RwlockList[tmpvector[j]]);
						}
					}
				}
			
				//apply
				vertex_Program->apply(VerticesList[CurrId], gatherValue, *EngineInfo);

				//unlock write lock on current vertex
				if(GatherDir == IN || GatherDir == OUT || GatherDir == BOTH)
				{
						pthread_rwlock_unlock(&RwlockList[CurrId]);
				}
		
				//scatter
				
				Engine_Scatter(CurrId);
			}
			
			if(dynamic)
			{
				pthread_mutex_lock(&EngineMutex);
				CurrId = NewJob;
				++NewJob;
				//cout<<"2: "<<CurrId<<endl;
				pthread_mutex_unlock(&EngineMutex);
			}
			else
			{
				++CurrId;
			}
		}
		pthread_barrier_wait(EngineBarrier);
	}
}
template<typename VertexType, typename EdgeType, typename GatherType>
void* Engine<VertexType, EdgeType, GatherType>::thread_worker_helper(void* arg)
{
	args<VertexType, EdgeType, GatherType>* argument = (args<VertexType, EdgeType, GatherType>*) arg;
	return ((Engine<VertexType, EdgeType, GatherType> *)(argument->context))->thread_worker(&argument->tid);
}
/*
template<typename VertexType, typename EdgeType, typename GatherType>
Engine<VertexType, EdgeType, GatherType>::~Engine()
{
	delete GraphData;
	delete EngineInfo;
	delete [] VerticesList;
}*/

#endif
