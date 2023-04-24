#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.c"

int main(int argc, char* argv[]) {


	pthread_t threads;
    struct args *args = (struct args*) malloc(sizeof(struct args));
    args->queue = create_task_queue();

    pthread_create(threads, NULL, startThread, (void*) args);



    return 0;
}


// TODO: pthread create - when doing this, dont forget to initialize the struct args first,
// so there will be no junk values