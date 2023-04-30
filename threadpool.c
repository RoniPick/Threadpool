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
	thread_pool *pool= (thread_pool*)malloc(sizeof(thread_pool));
    pool->num_threads = sysconf(_SC_NPROCESSORS_CONF); // get number of cores
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * pool->num_threads);
    pool->queue = create_task_queue();
    pthread_mutex_init(&pool->mutex, NULL); // init lock
    pthread_cond_init(&pool->cond, NULL); // init cond
    return pool;
}

Task *create_task(char* data, char* flag, int key){
	Task *newTask=malloc(sizeof(Task));
	if(newTask == NULL){
		return NULL;
	}
	memcpy(newTask->data, data, TASK_SIZE);
	newTask->flag = flag;
	newTask->key=key;

	return newTask;
}



// function to execute a task by encrypting or decrypting the data
void executeTask(task_queue* q,Task* task, thread_pool *tp) {
	if (strcmp(task->flag, "-e") == 0){
		encrypt(task->data, task->key);
	}
	else if (strcmp(task->flag, "-d") == 0){
		decrypt(task->data, task->key);
	}
    printf("%s", task->data);
    pthread_mutex_lock(&tp->mutex);
    free(dequeue(q)); //remove the task from the queue after executing it
    pthread_mutex_unlock(&tp->mutex);
}

// function to add a new task to the task queue
void submitTask(Task *task, thread_pool *tp) {
    pthread_mutex_lock(&tp->mutex); //lock the mutex to prevent other threads from accessing the queue
    enqueue(tp->queue, task);
    pthread_mutex_unlock(&tp->mutex);
    pthread_cond_signal(&tp->cond); //signal the waiting threads that a new task is available
}


// function to be executed by each thread in the thread pool
void* startThread(void* args) {
    struct args *a = (struct args*) args;
    while (1){
		Task *task;

		pthread_mutex_lock(&a->tp->mutex);
		while (a->tp->queue->size == 0){ //if there are no tasks-wait for a signal
			pthread_cond_wait(&a->tp->cond, &a->tp->mutex);
            if (a->status == 1)
            {
                pthread_mutex_unlock(&a->tp->mutex);
                printf("\nthread %ld exit\n", pthread_self());
                return NULL;
            }
        }

		task = a->tp->queue->head;
		pthread_mutex_unlock(&a->tp->mutex);
		executeTask(a->tp->queue, task, a->tp);
        printf("\nthread %ld done task\n", pthread_self());
	}
	return NULL;
}
