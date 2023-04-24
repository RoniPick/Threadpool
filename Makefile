.PHONY: all
all: task stdinExample threadpool.o

task:	codec.h basic_main.c
	gcc basic_main.c -L. -l Codec -o encoder

stdinExample:	stdin_main.c
		gcc stdin_main.c -L. -l Codec -o tester

threadpool.o: threadpool.c threadpool.h
		gcc -c threadpool.c


.PHONY: clean
clean:
	-rm encoder tester 2>/dev/null
