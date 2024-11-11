#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

int readers = 0;       
bool writing = false;  

sem_t mutex;           
sem_t ok_to_read;      
sem_t ok_to_write;     


void Start_Read() {
    sem_wait(&mutex);  
    if (writing || sem_trywait(&ok_to_write) == 0) {  
        sem_post(&ok_to_write);                       
        sem_wait(&ok_to_read);                        
    }
    readers++;  
    sem_post(&ok_to_read);  
    sem_post(&mutex);       
}


void End_Read() {
    sem_wait(&mutex);      
    readers--;             
    if (readers == 0) {
        sem_post(&ok_to_write);  
    }
    sem_post(&mutex);      
}


void Start_Write() {
    sem_wait(&mutex);       
    if (readers > 0 || writing) {
        sem_wait(&ok_to_write);  
    }
    writing = true;         
    sem_post(&mutex);       
}


void End_Write() {
    sem_wait(&mutex);           
    writing = false;            
    if (sem_trywait(&ok_to_read) == 0) {
        sem_post(&ok_to_read);  
    } else {
        sem_post(&ok_to_write); 
    }
    sem_post(&mutex);           
}

void* reader(void* arg) {
    int id = *((int*)arg);
    free(arg);
    Start_Read();
    printf("Leitor %d está lendo\n", id);
    End_Read();
    pthread_exit(NULL);
}

void* writer(void* arg) {
    int id = *((int*)arg);
    free(arg);
    Start_Write();
    printf("Escritor %d está escrevendo\n", id);
    End_Write();
    pthread_exit(NULL);
}

int main() {
    pthread_t readers_threads[5], writers_threads[5];
    sem_init(&mutex, 0, 1);
    sem_init(&ok_to_read, 0, 0);
    sem_init(&ok_to_write, 0, 0);

    for (int i = 0; i < 5; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers_threads[i], NULL, reader, id);
        id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers_threads[i], NULL, writer, id);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(readers_threads[i], NULL);
        pthread_join(writers_threads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&ok_to_read);
    sem_destroy(&ok_to_write);
    return 0;
}
