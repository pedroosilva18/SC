#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Atp3.h"

void *oddNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_odd;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the odd numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    file_descriptor_odd = open(ODD_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT , 0644);

    if (file_descriptor_odd == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        read_size = read(prod_file_descriptor, buffer, BUFFER_SIZE);

        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 != 0)
            {
                printf("Storing odd number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                write(file_descriptor_odd, buffer, BUFFER_SIZE);

                // TODO - Store formated odd number into the odd numbers file
            }
        }
        usleep(250000);
    }

    return NULL;
}

void *evenNumbersConsumer(void *vargp)
{
    int file_descriptor = *(int *)vargp;
    int file_descriptor_even;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;



    file_descriptor_even = open(EVEN_NUMBERS_FILE_NAME, O_WRONLY | O_CREAT , 0644);

    // TODO - Open the even numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    if (file_descriptor_even == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        read_size = read(file_descriptor, buffer, BUFFER_SIZE);

        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.

        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 == 0)
            {
                printf("Storing even number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                
                // TODO - Store formated even number into the even numbers file
                write(file_descriptor_even, buffer, BUFFER_SIZE);
            }
        }
        usleep(250000);
    }

    return NULL;
}

int main()
{
    pthread_t tid_1, tid_2;


    int file_descriptor1 = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT , 0644);
    int file_descriptor2 = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT , 0644);



    if ((file_descriptor1 == -1) || (file_descriptor2 == -1))
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    pthread_create(&tid_2, NULL, oddNumbersConsumer,  (void*) &file_descriptor1);
    pthread_create(&tid_2, NULL, evenNumbersConsumer,  (void*) &file_descriptor2);

    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    exit(0);
}
