#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

int main(int argc, char *argv[]){

    // input validation
    if (argc != 3){
        printf("invalid input");
        exit(0);
    }

    char data[TASK_SIZE] = {'\0'};
    char *flag;
    int key;

    // get the data from the input args
    key = atoi(argv[1]);
    flag = argv[2];

    // input validation
    if (strcmp(flag, "-e") && strcmp(flag, "-d")){
        printf("invalid input");
        exit(0);
    }

    thread_pool *tp = create_thread_pool();
    int threads_num = tp->num_threads; // the number of the computer's core
    struct args args = {.queue = tp->queue, .tp = tp, .status = 0};

    // creating threads 
    for (int i = 0; i < threads_num; i++){
        if (pthread_create(&tp->threads[i], NULL, &startThread, &args) != 0){
            printf("failing create thread");
            exit(0);
        }
    }

    int b = 0;
    int index = 0;
    
    //for printing
    Task *tasks[TASK_SIZE];
    int tasks_count = 0;
    
    // reading the data from the file
    while ((b = read(0, data, TASK_SIZE))){
        Task *task = create_task(data, flag, key , b);
        task->idx = index++;
        submitTask(task, tp);
        memset(data, 0, TASK_SIZE);
    }

    args.status = 1; // done!

    pthread_cond_broadcast(&tp->cond);
    // this will wake up all threads waiting on cond
    for (int i = 0; i < threads_num; i++){
        if (pthread_join(tp->threads[i], NULL) != 0){
            printf("failing joining thread");
            exit(0);
        }
    }
    
     //print the tasks in the order they were enqueued
    for (int i = 0; i < tasks_count; i++) {
        write(1, tasks[i]->data, tasks[i]->size);
        
    }

    free(tp);
    return 0;
}
