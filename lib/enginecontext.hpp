#ifndef __ENGINE_CONTEXT_HPP__
#define __ENGINE_CONTEXT_HPP__

#include "enginecontext.h"

EngineContext::EngineContext(IdType NumId)
{
	//reference_data data;
	CurrIterationNum = 0;
	TaskScheduler = new BitsetScheduler(NumId);
}

int EngineContext::iteration()
{
	return CurrIterationNum;
}

BitsetScheduler& EngineContext::scheduler()
{
	return *TaskScheduler;
}

void EngineContext::iterationIncrement()
{
	CurrIterationNum++;
}

EngineContext::~EngineContext()
{
	delete TaskScheduler;
}
#endif
