/*
 * enginecontext.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __ENGINE_CONTEXT_H__
#define __ENGINE_CONTEXT_H__

#include "bitsetscheduler.hpp"


class EngineContext {
  public:
    EngineContext(IdType NumId);
    int iteration();
    BitsetScheduler& scheduler();
	void iterationIncrement();
	~EngineContext();
  private:
	int CurrIterationNum;
	BitsetScheduler* TaskScheduler;
};

#endif /* __ENGINE_CONTEXT_H__ */
