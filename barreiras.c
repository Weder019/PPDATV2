#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5


int count = 0;                 
sem_t mutex;                  
sem_t barrier_sem;            


void barrier() {
    sem_wait(&mutex);         
    count++;

    if (count == NUM_THREADS) {
        
        for (int i = 0; i < NUM_THREADS; i++) {
            sem_post(&barrier_sem);
        }
        count = 0; 
    }

    sem_post(&mutex);         
    sem_wait(&barrier_sem);   
}


void* thread_function(void* arg) {
    int id = *((int*)arg);
    free(arg);

    printf("Thread %d chegou na barreira\n", id);
    barrier(); 
    printf("Thread %d passou pela barreira\n", id);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];


    sem_init(&mutex, 0, 1);
    sem_init(&barrier_sem, 0, 0);


    for (int i = 0; i < NUM_THREADS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&threads[i], NULL, thread_function, id);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&barrier_sem);

    return 0;
}
