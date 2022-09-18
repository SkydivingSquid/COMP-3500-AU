#see README for a manual compilation process
CC = g++ # g++ compiler
OBJ = compute.o method.o open.o print.o read.o # all required object files

scheduler: $(OBJ) scheduler.cpp scheduler.hpp # for compiling scheduler
	@echo "Creating scheduler"
	$(CC) -o $@ $^ 

clean:
	@echo "Deleting object and binary files"
	rm -rvf *.o *.exe scheduler
