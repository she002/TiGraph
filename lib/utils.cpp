/*
 * utils.cpp
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#include "utils.h"
#include <unistd.h>
#include <sys/time.h>

std::map<size_t, std::string> typeToFormatSpecifier;

int nThreads = 1;

void initUtils(int *argc, char ***argv) {
  typeToFormatSpecifier[typeid(int).hash_code()] = "%d";
  typeToFormatSpecifier[typeid(float).hash_code()] = "%f";
  typeToFormatSpecifier[typeid(IdType).hash_code()] = "%lu";
  typeToFormatSpecifier[typeid(char).hash_code()] = "%s";

  parse(argc, *argv, "--engine-nthreads=", &nThreads);
  fprintf(stderr, "Setting nthreads = %d\n", nThreads);
}

void removeArg(int *argc, char **argv, int i) {
  if (i < *argc)
    (*argc)--;
  for (; i < *argc; i++)
    argv[i] = argv[i + 1];
}

double getTimer() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000.0;
}

