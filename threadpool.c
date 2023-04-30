#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

task_queue queue = {.head = NULL, .tail = NULL, .size = 0}; // define a task_queue struct
int taskCount = 0; // variable to keep track of the number of tasks

// // mutex and condition variable for thread synchronization
// pthread_mutex_t mutexQueue;
// pthread_cond_t condQueue;


task_queue *create_task_queue() {
    task_queue *queue = (task_queue*)malloc(sizeof(task_queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

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
	thread_pool *pool=malloc(sizeof(thread_pool));
    pool->num_threads = sysconf(_SC_NPROCESSORS_CONF); // get number of cores
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * pool->num_threads);
    pool->queue = create_task_queue();
    return pool;
}

Task *create_task(char* data, char* flag, int key){
	Task *newTask=malloc(sizeof(Task));
	if(newTask == NULL){
		return NULL;
	}
	newTask->data = data;
	newTask->flag = flag;
	newTask->key=key;

	return newTask;
}



// function to execute a task by encrypting or decrypting the data
void executeTask(task_queue* q,Task* task) {
	if (strcmp(task->flag, "-e") == 0){
		encrypt(task->data, task->key);
	}
	else if (strcmp(task->flag, "-d") == 0){
		decrypt(task->data, task->key);
	}
    printf("%s", task->data);
    free(dequeue(q)); //remove the task from the queue after executing it
}

// function to add a new task to the task queue
void submitTask(Task *task, thread_pool *tp) {
    pthread_mutex_lock(&tp->mutex); //lock the mutex to prevent other threads from accessing the queue
    enqueue(&queue, task);
    taskCount++;
    pthread_mutex_unlock(&tp->mutex);
    pthread_cond_signal(&tp->cond); //signal the waiting threads that a new task is available
}


// function to be executed by each thread in the thread pool
void* startThread(void* args) {
    struct args *a = (struct args*) args;
    while (taskCount > 0){
		Task *task;

		pthread_mutex_lock(&a->tp->mutex);
		while (taskCount == 0) //if there are no tasks-wait for a signal
			pthread_cond_wait(&a->tp->cond, &a->tp->mutex);

		task = queue.head;
		taskCount--;
		pthread_mutex_unlock(&a->tp->mutex);
		executeTask(a->queue, task);
	}
	return 0;
}
