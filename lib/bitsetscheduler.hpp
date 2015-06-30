// NOTE: This source is taken from GraphChi 1.0

/**
 * @file
 * @author  Aapo Kyrola <akyrola@cs.cmu.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright [2012] [Aapo Kyrola, Guy Blelloch, Carlos Guestrin / Carnegie Mellon University]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 *
 * @section DESCRIPTION
 *
 * Bitset scheduler.
 */

#ifndef __BITSET_SCHEDULER_HPP__
#define __BITSET_SCHEDULER_HPP__

#include "densebitset.hpp"

class BitsetScheduler {
  private:
    dense_bitset * curiteration_bitset;
    dense_bitset * nextiteration_bitset;
  public:
    bool has_next_tasks;

	BitsetScheduler()
	{
	  curiteration_bitset = nullptr;
      nextiteration_bitset = nullptr;
	  has_next_tasks = false;
	}
    BitsetScheduler(IdType nvertices) {
      curiteration_bitset = new dense_bitset(nvertices);
      nextiteration_bitset = new dense_bitset(nvertices);

      curiteration_bitset->clear();
      nextiteration_bitset->clear();

      has_next_tasks = false;
    }

    void new_iteration() {
      dense_bitset * tmp = curiteration_bitset;
      curiteration_bitset = nextiteration_bitset;
      nextiteration_bitset = tmp;
      nextiteration_bitset->clear();
      has_next_tasks = false;
    }

    virtual ~BitsetScheduler() {
      delete nextiteration_bitset;
      delete curiteration_bitset;
    }

    inline void add_task(IdType vertex, bool also_this_iteration = false) {
      nextiteration_bitset->set_bit(vertex);
      if (also_this_iteration) {
        curiteration_bitset->set_bit(vertex);
      }
      has_next_tasks = true;
    }

    inline void add_task_in_current(IdType vertex) {
      curiteration_bitset->set_bit(vertex);
    }

    inline void add_task_if_not_in_current(IdType vertex) {
      if(curiteration_bitset->get(vertex))
        return;
      nextiteration_bitset->set_bit(vertex);
      has_next_tasks = true;
    }

    void resize(IdType maxsize) {
      curiteration_bitset->resize(maxsize);
      nextiteration_bitset->resize(maxsize);

    }

    inline bool is_scheduled(IdType vertex) {
      return curiteration_bitset->get(vertex);
    }

    void remove_tasks(IdType fromvertex, IdType tovertex) {
      nextiteration_bitset->clear_bits(fromvertex, tovertex);
    }

    void add_task_to_all(bool also_this_iteration = false) {
      nextiteration_bitset->setall();
      if (also_this_iteration) {
        curiteration_bitset->setall();
      }
      has_next_tasks = true;
    }

    IdType num_tasks() {
      return curiteration_bitset->countSetBits();
    }

    bool anyScheduledTasks() {
      return has_next_tasks;
    }

    dense_bitset* getCurrentBitset() const {
      return curiteration_bitset;
    }

};

#endif // __BITSET_SCHEDULER_HPP__
