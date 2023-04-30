FLAGS = -g -Wall -L.
LFLAGS = -Wl,-rpath='./' -lCodec -lpthread


.PHONY: all
all:  coder

# task:	codec.h basic_main.c
# 	gcc basic_main.c -L. -l Codec -o encoder

# stdinExample:	stdin_main.c
# 		gcc stdin_main.c -L. -l Codec -o tester

threadpool.o: threadpool.c threadpool.h
		gcc $(FLAGS) -c threadpool.c -o threadpool.o $(LFLAGS)

coder: main.c threadpool.o codec.h
		gcc $(FLAGS) main.c threadpool.o -o encoder $(LFLAGS)

.PHONY: clean
clean:
	rm -f encoder tester *.o

# LFLAGS = -Wl,-rpath='./' -lCodec -lpthread