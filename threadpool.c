#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "threadpool.h"

#define TASK_SIZE 1024

/* This function creates a new task queue and returns a pointer to it. */
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

/* This function takes a task as input and frees the memory allocated to it. */
void task_destroy(Task *task){
	if(task==NULL){
		return;
	}
	free(task);
}

/* This function takes a task queue and a task as input and adds the task to the tail of the queue. */
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

/* This function takes a task queue as input and removes and returns the task at the head of the queue. */
Task *dequeue(task_queue *queue) {
    if (queue->size == 0) {
        return NULL;
    }
    Task *task = queue->head;
    queue->head = queue->head->next;
    queue->size--;
    return task;
}

/* This function creates a new thread pool and returns a pointer to it. It sets the num_threads field to the number of available cores on the system, 
creates an array of pthread_t structures for the threads, and initializes the mutex and cond variables. */
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

/* This function creates a new task and returns a pointer to it. It takes four parameters: a character array data that holds the data to be processed, 
a character pointer flag that holds the operation to be performed (either encryption or decryption), 
an integer key that holds the encryption/decryption key, and an integer size that holds the size of the data. */
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

/* This function takes a task as input and executes it. It performs either encryption or decryption on the data field of the task depending 
on the value of the flag field. */
void executeTask(Task *task){
    if (strcmp(task->flag, "-e") == 0){
        encrypt(task->data, task->key);
    }
    else if (strcmp(task->flag, "-d") == 0){
        decrypt(task->data, task->key);
    }
    //write(1, task->data, task->size);
}

/* This function takes a task and a thread pool as input and adds the task to the task queue. It first locks the mutex to prevent other threads from accessing the queue, 
then adds the task to the queue, signals the waiting threads that a new task is available, and unlocks the mutex. */
void submitTask(Task *task, thread_pool *tp){
    pthread_mutex_lock(&tp->mutex); // lock the mutex to prevent other threads from accessing the queue
    enqueue(tp->queue, task);
    pthread_cond_signal(&tp->cond); // signal the waiting threads that a new task is available
    pthread_mutex_unlock(&tp->mutex);
}

/* This function is the main function executed by each worker thread in the thread pool. 
It takes a structure args as input, which contains a pointer to the thread pool and a status variable that indicates whether the thread should exit. 
It retrieves tasks from the task queue and executes them until the status variable is set to 1, indicating that the thread should exit. */
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
