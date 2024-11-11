#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem1;
sem_t sem2;

void* thread_func1(void* arg) {
    sem_wait(&sem1);
    printf("Thread 1: bloqueou sem1\n");
    sleep(1);  

    printf("Thread 1: tentando bloquear sem2\n");
    sem_wait(&sem2);  
    printf("Thread 1: bloqueou sem2\n");

    sem_post(&sem2);
    sem_post(&sem1);

    return NULL;
}

void* thread_func2(void* arg) {
    sem_wait(&sem2);
    printf("Thread 2: bloqueou sem2\n");
    sleep(1);  

    printf("Thread 2: tentando bloquear sem1\n");
    sem_wait(&sem1);  
    printf("Thread 2: bloqueou sem1\n");

    sem_post(&sem1);
    sem_post(&sem2);

    return NULL;
}

int main() {
    pthread_t t1, t2;

    
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 1);

    
    pthread_create(&t1, NULL, thread_func1, NULL);
    pthread_create(&t2, NULL, thread_func2, NULL);

    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

   
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}
