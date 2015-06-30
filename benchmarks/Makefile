PR_TARGET       := pagerank
SSSP_TARGET     := sssp

LIB_PATH      := ../lib

CPP        := g++ -std=c++11
#CC         := gcc
RM          := rm -rf

# Flags
CFLAGS      := -g -pthread #-Wall -MMD -O3

OBJS        := $(LIB_PATH)/utils.o
PR_OBJS       := $(OBJS) pagerank.o
SSSP_OBJS       := $(OBJS) sssp.o

all:        $(PR_TARGET) 
#$(SSSP_TARGET) 

$(PR_TARGET): $(PR_OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) $(RPATH) $(INC) -o $@

$(SSSP_TARGET): $(SSSP_OBJS)
	$(CPP) $(CFLAGS) $^ $(LIBS) $(RPATH) $(INC) -o $@

%.o: %.cpp %.h 
	$(CPP) $(CFLAGS) $(LIBS) $(RPATH) $(INC) -c $< -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) $(LIBS) $(RPATH) $(INC) -c $< -o $@

clean:
	$(RM) *.o $(LIB_PATH)/*.o $(PR_TARGET) $(SSSP_TARGET) *.d $(LIB_PATH)/*.d
run1:
	./$(PR_TARGET) --pagerank-graphfile=../inputs/com-amazon.ungraph.txt --pagerank-niters=100 --pagerank-outputfile=multi_dynamic.txt --pagerank-dynamic-tasks-allocation=no --engine-nthreads=1

run2:
	./$(SSSP_TARGET) --sssp-graphfile=../inputs/com-amazon.ungraph.txt --sssp-niters=100 --sssp-outputfile=sssp_single.txt --sssp-source=1 --pagerank-dynamic-tasks-allocation=no --engine-nthreads=1
	

