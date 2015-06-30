CPP	:= g++ -std=c++11
CFLAGS	:= -g -Wall -MMD -O0
INDIR	:= ../inputs

pagerank:	test.cpp
	$(CPP) $(CFLAGS) test.cpp -o pagerank.out
run:
	./pagerank.out $(INDIR)/input.txt
clean:
	rm sample.txt *.out *.o *~
