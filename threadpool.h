#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#define TASK_SIZE 1024

typedef struct task {
    int key;
    char data[TASK_SIZE];
    int flag;
    int task_num;
    struct task *next;
} task;

typedef struct task_queue {
    task *head;
    task *tail;
    int size;
} task_queue;

typedef struct thread{
    pthread_t thread;
    int thread_status;
} thread;

typedef struct thread_pool {
    int num_threads;
    pthread_t *threads;
    task_queue *queue;
} thread_pool;


task_queue *create_task_queue() {
    task_queue *queue = (task_queue*)malloc(sizeof(task_queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(task_queue *queue, task *new_task) {
    if (queue->size == 0) {
        queue->head = new_task;
        queue->tail = new_task;
    } 
    else {
        queue->tail->next = new_task;
        queue->tail = new_task;
    }
    queue->size++;
}

task *dequeue(task_queue *queue) {
    if (queue->size == 0) {
        return NULL;
    }
    task *task = queue->head;
    queue->head = queue->head->next;
    queue->size--;
    return task;
}

thread_pool *create_thread_pool(thread_pool *pool){
    pool->num_threads = sysconf(_SC_NPROCESSORS_CONF); // get number of cores
    thread_pool *pool = (thread_pool*)malloc(sizeof(thread_pool));
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * pool->num_threads);
    pool->queue = create_task_queue();
    return pool;
}



