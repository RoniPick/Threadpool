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
    struct args args = {.queue = tp->queue, .tp = tp, .status = 0};

    for(int i=0; i<num; i++){        
        if(pthread_create(&tp->threads[i], NULL, &startThread, &args) != 0){
            printf("failing create thread");
            exit(0);
        }
    }

    char c;
	int counter = 0;
    int indx=0;

	while ((c = getchar()) != EOF){
	  data[counter] = c;
	  counter++;

	  if (counter == 1024){
        Task *t = create_task(data, flag, key); 
        t->idx=indx++;
        submitTask(t, tp);
		counter = 0;
        memset(data, '\0', TASK_SIZE);
        printf("\n%d\n", tp->queue->size);
	  }
	}
    if (counter > 0){
        Task *t = create_task(data, flag, key);
        t->idx=indx++;
        submitTask(t, tp);
        printf("\n%d\n", tp->queue->size);
		counter = 0;
        memset(data, '\0', TASK_SIZE);
	  }

      
    while (tp->queue->size > 0){
        pthread_mutex_lock(&tp->mutex);
        executeTask(tp->queue, tp->queue->head, tp);
        // pthread_mutex_unlock(&tp->mutex);
        usleep(500000);
        continue;
    }
    args.status = 1; // done!

    // printf("\nMAIN\n");
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
