#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include "codec.h"

#define TASK_SIZE 1024


typedef struct Task{
	int idx;
    char data[TASK_SIZE];
    char* flag;
    int key;
    int status;
    int size;
    struct Task *next;
} Task;

typedef struct task_queue {
    Task *head;
    Task *tail;
    int size;
} task_queue;

typedef struct thread_pool {
    int num_threads;
    pthread_t *threads;
    task_queue *queue;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} thread_pool;

typedef struct args {
    task_queue *queue;
    thread_pool *tp;
    int status;
} args;


void executeTask(Task *task);
void printdata();
void submitTask(Task *task, thread_pool *tp);
void* startThread(void* args);
Task *create_task(char *data, char *flag, int key, int size);
void enqueue(task_queue *queue, Task *new_task);
Task *dequeue(task_queue *queue);
task_queue *create_task_queue();
thread_pool *create_thread_pool();
void task_destroy(Task *task);

#endif