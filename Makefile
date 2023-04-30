FLAGS = -g -Wall -L.
LFLAGS = -Wl,-rpath='./' -lCodec -lpthread


.PHONY: all
all:  coder

threadpool.o: threadpool.c threadpool.h
		gcc $(FLAGS) -c threadpool.c -o threadpool.o $(LFLAGS)

coder: main.c threadpool.o codec.h
		gcc $(FLAGS) main.c threadpool.o -o coder $(LFLAGS)

.PHONY: clean
clean:
	rm -f coder tester *.o
