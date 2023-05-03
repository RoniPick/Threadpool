#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

task_queue *create_task_queue() {
    task_queue *queue = (task_queue*)malloc(sizeof(task_queue));
    if(queue==NULL){
        printf("error creating task_queue");
        exit(0);
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

// function that destroy the task after done
void task_destroy(Task *task){
	if(task==NULL){
		return;
	}
	free(task);
}

void enqueue(task_queue *queue, Task *new_task) {
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

Task *dequeue(task_queue *queue) {
    if (queue->size == 0) {
        return NULL;
    }
    Task *task = queue->head;
    queue->head = queue->head->next;
    queue->size--;
    return task;
}


thread_pool *create_thread_pool(){
	thread_pool *pool = (thread_pool*)malloc(sizeof(thread_pool));
    if(pool==NULL){
        printf("error creating threadpool");
        exit(0);
    }
    pool->num_threads = sysconf(_SC_NPROCESSORS_CONF); // get number of cores
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * pool->num_threads);
    pool->queue = create_task_queue();
    pthread_mutex_init(&pool->mutex, NULL); // init lock
    pthread_cond_init(&pool->cond, NULL); // init cond
    return pool;
}


Task *create_task(char *data, char *flag, int key, int size){
    Task *newTask = malloc(sizeof(Task));
    if (newTask == NULL){
        printf("error creating task");
        return NULL;
    }
    memcpy(newTask->data, data, TASK_SIZE);
    newTask->flag = flag;
    newTask->key = key;
    newTask->size = size;

    return newTask;
}


// function to execute a task by encrypting or decrypting the data
void executeTask(Task *task){
    if (strcmp(task->flag, "-e") == 0){
        encrypt(task->data, task->key);
    }
    else if (strcmp(task->flag, "-d") == 0){
        decrypt(task->data, task->key);
    }
    write(1, task->data, task->size);
}


// function to add a new task to the task queue
void submitTask(Task *task, thread_pool *tp){
    pthread_mutex_lock(&tp->mutex); // lock the mutex to prevent other threads from accessing the queue
    enqueue(tp->queue, task);
    pthread_cond_signal(&tp->cond); // signal the waiting threads that a new task is available
    pthread_mutex_unlock(&tp->mutex);
}


// function to be executed by each thread in the thread pool
void *startThread(void *args){
    struct args *a = (struct args *)args;
    Task *task;

    while (1){
        pthread_mutex_lock(&a->tp->mutex);
        while (a->tp->queue->size == 0){ // if there are no tasks-wait for a signal
            if (a->status == 1){
                pthread_mutex_unlock(&a->tp->mutex);
                // printf("\nthread %ld exit\n", pthread_self());
                return NULL;
            }
            pthread_cond_wait(&a->tp->cond, &a->tp->mutex);
        }

        task = dequeue(a->tp->queue); // remove the task from the queue after executing it
        pthread_mutex_unlock(&a->tp->mutex);
        executeTask(task);
        task_destroy(task); // destroy the task
        // printf("\nthread %ld done task\n", pthread_self());
    }
    return NULL;
}