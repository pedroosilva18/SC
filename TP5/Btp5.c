#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "p5_helper.h"

#define BUFFER_MAX_SIZE 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int data_space[BUFFER_MAX_SIZE];
circ_buff_t buffer = { 
    data_space,
    BUFFER_MAX_SIZE,
    0,
    0
};

void* producer(void *arg)
{
    unsigned int push_value;
    while (1)
     {
        push_value = (rand() % 1000); //random [0,999]

        pthread_mutex_lock(&mutex);
        if (circ_buff_push(&buffer, push_value ) == 0)
            printf("Producer: %u\n", push_value);
        else
            printf("Producer: buffer is full\n"); 
        
         pthread_mutex_unlock(&mutex);
        usleep(100*1000); //100 ms
    }
    
    return NULL;
}

void* consumer(void *arg)
{
    unsigned int pop_value;
    while (1) 
    {
        pthread_mutex_lock(&mutex);
        if (circ_buff_pop(&buffer, &pop_value)==0)
            printf("                              Consumer: returned %u\n", pop_value);
        else
            printf("                              Consumer: buffer is empty\n");

         pthread_mutex_unlock(&mutex);
        usleep(150*1000); //150 ms
    }
    
    return NULL;
}
int main(void)
{
    //Seeding...
	srand(time(NULL));

    int lel = pthread_mutex_init(&mutex, NULL);
    if (lel != 0)
    {
        puts("MUTEX WASNT CREATED");
        exit(-1);
    }


    pthread_t tid[2];
    
	pthread_create(&(tid[0]), NULL, &producer, NULL);
	pthread_create(&(tid[1]), NULL, &consumer, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

 if (pthread_mutex_destroy(&mutex) != 0)
    {
        puts("MUTEX WASNT DESTROYED");
        exit(-1);
    }

    return 0;
}