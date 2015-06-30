/*
 * utils.h
 *
 *  Created on: Apr 05, 2015
 *      Author: keval
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <map>
#include <typeinfo>
#include <cstring>
#include <climits>

typedef unsigned long int IdType;
#define MAXID ULONG_MAX

typedef struct empty { } Empty;

extern std::map<size_t, std::string> typeToFormatSpecifier;
extern int nThreads;

void initUtils(int *argc, char ***argv);
void removeArg(int *argc, char **argv, int i);
double getTimer();

template<typename ParseType>
bool parse(int* argc, char** argv, const char* str, ParseType* value) {
  int siz = strlen(str);
  for (int i = 1; i < *argc; i++)
    if (strncmp(argv[i], str, siz) == 0) {
      sscanf(argv[i] + siz, typeToFormatSpecifier[typeid(ParseType).hash_code()].c_str(), value);
      removeArg(argc, argv, i);
      return true;
    }
  return false;
}

#endif /* __UTILS_H__ */
