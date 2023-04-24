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

// mutex and condition variable for thread synchronization
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;


// function to execute a task by encrypting or decrypting the data
void executeTask(task_queue* q,Task* task) {
	if (strcmp(task->flag, "-e") == 0){
		encrypt(task->data, task->flag);
		// printf("%s", task->buffer);
	}
	else if (strcmp(task->flag, "-d") == 0){
		decrypt(task->data, task->flag);
		// printf("%s", task->buffer);
	}
    printf("%s", task->data);
    dequeue(q); //remove the task from the queue after executing it
}

// function to add a new task to the task queue
void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue); //lock the mutex to prevent other threads from accessing the queue
    enqueue(&queue, &task);
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue); //signal the waiting threads that a new task is available
}


// function to be executed by each thread in the thread pool
void* startThread(void* args) {
    struct args *a = (struct args*) args;
    while (taskCount > 0){
		Task *task;

		pthread_mutex_lock(&mutexQueue);
		while (taskCount == 0) //if there are no tasks-wait for a signal
			pthread_cond_wait(&condQueue, &mutexQueue);

		task = queue.head;
		taskCount--;
		pthread_mutex_unlock(&mutexQueue);
		executeTask(a->queue, task);
	}
	return 0;
}
