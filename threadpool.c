#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

task_queue queue = {.head = NULL, .tail = NULL, .size = 0};
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

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
    dequeue(q);
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    enqueue(&queue, &task);
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
    struct args *a = (struct args*) args;
    while (taskCount > 0){
		Task *task;

		pthread_mutex_lock(&mutexQueue);
		while (taskCount == 0)
			pthread_cond_wait(&condQueue, &mutexQueue);

		task = queue.head;

		taskCount--;
		pthread_mutex_unlock(&mutexQueue);
		executeTask(a->queue, task);
	}
	return 0;
}
