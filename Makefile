.PHONY:clean
CC = g++

vpath %.h include
vpath %.cpp src

#CFLAGS = -g -fopenmp -I include -lpthread -std=c++11 -Ofast
CFLAGS = -g -I include -std=c++11 -Ofast
OBJ = obj/kgraph.o obj/utils.o obj/main.o
main:$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
obj/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f obj/*.o main

