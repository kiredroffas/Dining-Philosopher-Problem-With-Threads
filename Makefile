.PHONY: clean run #Tell make that not associated with building files

#Compile source file and produce executable
all : threads.o
	cc -o threads threads.c -lm -pthread -w
#Runs compiled executable
run :
	./threads
#Remove object and temp files
clean :
	rm threads *.o
