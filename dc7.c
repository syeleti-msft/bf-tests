#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#define BUF_SIZE 1024
char* get_rand_data_buf();

// This program describes delete/unlink of a file when the file opened is causing undefined behaviour with block cache.

int main() {
    int writeFD;
    char* buffer = get_rand_data_buf(); // intialising the buffer with some garbage data
    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c2/foo", O_WRONLY| O_TRUNC | O_CREAT, 0777);
    // Write to the file
    bytesWritten = write(writeFD, buffer, 1024);

    // Delete the file
    if (unlink("c2/foo") == -1) {
        perror("Deletion of the file failed");
    } else {
        printf("File is deleted successfully\n");
    }
    close(writeFD);

    
    // Now after the Close is finished there should be no file remaining in the storage account as we deleted it.
    // lets check if the file presence in the account. 
    struct stat st;
    if (stat("c2/foo", &st) == 0) {
        printf("Oops file is present after deletion\n");
    } else {
        perror("stat failed");
    }

    // But this close is actually triggering the blobfuse to again create the file on the Azure storage.
    return EXIT_SUCCESS;
}

char* get_rand_data_buf() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long current_time = ts.tv_nsec;
    srandom(current_time);
    char* allocatedMemory = (char *) malloc(BUF_SIZE);

    for (int bufferIndex = 0; bufferIndex < BUF_SIZE; bufferIndex++) {
        uint8_t randomNumber = (uint8_t) random();
        allocatedMemory[bufferIndex] = randomNumber;
    }
    return allocatedMemory;
}

/* 
->Output of the program by blobfuse block cache:
File is deleted successfully
Oops file is present after deletion

->Output of the program by Std file system:
File is deleted successfully
stat failed : No such file or directory

*/