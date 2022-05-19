#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Atp3.h"

void *numbersProducer(void *vargp)
{
    unsigned int number, fd =  *(int*)vargp;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        number = rand() % 1000;
        printf("Generated number %u\n", number);
        
        snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
        
        write(fd, buffer, BUFFER_SIZE);

        sleep(1);
    }

    return NULL;
}

int main()
{
    int file_descriptor = -1;
    pthread_t tid_1;

    file_descriptor = open(PRODUCER_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND , 0644);

    // TODO - Open file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, append data (don't overwrite file)
    // - set permissions to 0644

    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }
    //printf("%d\n", file_descriptor);

    // Modify the following statement if necessary (hint: it is!)
    pthread_create(&tid_1, NULL, numbersProducer, (void*) &file_descriptor  );

    pthread_join(tid_1, NULL);

    exit(0);
}
