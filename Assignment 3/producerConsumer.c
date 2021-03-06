/* Oscar Esparza
** CSC 139 : Operating System Principles
** 23 March 2019
** Assignment 3: Process Synchronization
*/

#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int ptr;
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// insert item into buffer
// return 0 if successful, otherwise
// return -1 indicating error
int insertItem(buffer_item item){
    if(ptr < BUFFER_SIZE) {
        buffer[ptr] = item;
        ptr++;
        return 0;
    }
    else return -1;
}

// remove an object from the buffer
// placing it in item
// return 0 if successful, otherwise
// return -1 indicating an error condition
int removeItem(buffer_item *item){
    if(ptr > 0){
        *item = buffer[0];
        
        for(int i = 0; i < (BUFFER_SIZE - 1); i++)
            buffer[i] = buffer[i + 1];
        
        ptr--;
        return 0;
    }
    else return -1;
}

void *producer(void *param){
    buffer_item item;
    unsigned int result = param;
    
    do {
        // sleep for a random amount of time
        sleep((rand_r(&result) % 10));
        // generate a random #
        item = (rand_r(&result) % 1000);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if (insertItem(item))
            printf("report error condition");
        else
            printf("producer produced: %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    } while(1);
}

void *consumer(void *param){
    buffer_item item;
    unsigned int result = param + 5;

    do {
        // sleep for a random amount of time
        sleep((rand_r(&result) % 10));
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if(removeItem(&item))
            printf("report error condition");
        else
            printf("consumer consumed: %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    } while(1);
}

int main(int argc, char *argv[]){
    pthread_t tid;
    pthread_attr_t attr;
    sem_t sem;
    int sleepValue, producerNumber, consumerNumber;

    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    if(argc != 4){
        printf("Error, you need 4 arguments!.\n");
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%d", &sleepValue);
    sscanf(argv[2], "%d", &producerNumber);
    sscanf(argv[3], "%d", &consumerNumber);

    /* 2. Initialize buffer */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    ptr = 0;

    /* 3. Create producer thread(s) */
    for (int i = 0; i < producerNumber; i++){
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, producer, i);
    }

    /* 4. Create consumer thread(s) */
    for(int i = 0; i < consumerNumber; i++){
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, consumer, i);
    }

    /* 5. Sleep */
    sleep(sleepValue);

    /* 6. Exit */
    exit(EXIT_SUCCESS);
}