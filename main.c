#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("invalid input");
        exit(0);
    }

    char data[TASK_SIZE]={'\0'};
    char* flag;
    int key;

    // get the data from the input args
	key = atoi(argv[1]);
	flag = argv[2];

    if(strcmp(flag, "-e") && strcmp(flag, "-d")){
        printf("invalid input");
        exit(0);
    }

    thread_pool *tp = create_thread_pool();
    int num=tp->num_threads;
    struct args *args = (struct args*) malloc(sizeof(struct args));
    args->queue = tp->queue;

    for(int i=0; i<num; i++){        
        if(pthread_create(&tp->threads[i], NULL, startThread, (void*) args) != 0){
            printf("failing create thread");
            exit(0);
        }
    }

    char c;
	int counter = 0;

	while ((c = getchar()) != EOF){
	  data[counter] = c;
	  counter++;

	  if (counter == 1024){
        Task *t=malloc(sizeof(Task));
        t->key=key;
        memcpy(t->data, data, TASK_SIZE);
        submitTask(t, tp);
		counter = 0;
        memset(data, '\0', TASK_SIZE);
	  }
	}

    for(int i=0; i<num; i++){
        pthread_cond_signal(&tp->cond);
    }

    for(int i=0; i<num; i++){
        if(pthread_join(tp->threads[i], NULL)!=0){
            printf("failing joining thread");
            exit(0);
        }
    }


    free(tp);
    return 0;
}
