#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#define BUF_SIZE 1024
char* get_rand_data_buf();
// This program simulates data corruption issue for blobfuse block cache when file is accessed using more than one file description.
// let "c1" is our mountpoint

int main() {
    int readFD;
    int writeFD;
    char* buffer = get_rand_data_buf();
    char* readbuf = get_rand_data_buf();//just intialising the buffers with some garbage data

    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c1/foo", O_WRONLY| O_TRUNC | O_CREAT, 0777);
    // Open the same file for reading.
    readFD = open("c1/foo", O_RDONLY, 0777);
    
    printf("read fd : %d, write fd: %d\n", readFD, writeFD);

    // Write at the start of the file
    bytesWritten = write(writeFD, buffer, BUF_SIZE);

    // Read from the start of the file
    bytesRead = read(readFD, readbuf, BUF_SIZE);

    printf("bytes written %ld, bytes read %ld\n", bytesWritten, bytesRead);
    // if the file foo is already present before running this program and having some data, 
    //    then this read will be success serving the past data that was present not the data that we have written in line 20.
    //    this is a corruption as when user open they are getting some inconsistent data. This data may be written somewhere else who knows.
    // else this call will return 0 bytes as file is created just now but the write is not reflected.

    // Both the cases are wrong and the following assertion would fail.
    assert(strcmp(buffer, readbuf) == 0);

    // Close the file handles
    close(readFD);
    close(writeFD);

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
Output of the program by block cache[RUN 1]:
read fd : 4, write fd: 3
bytes written 1024, bytes read 0
dc1: dc1.c:43: main: Assertion `strcmp(buffer, readbuf) == 0' failed.
Aborted (core dumped)

Output of the program by block cache[RUN 2]: Now the file is present in the Azure storage
read fd : 4, write fd: 3
bytes written 1024, bytes read 1024
dc1: dc1.c:43: main: Assertion `strcmp(buffer, readbuf) == 0' failed.
Aborted (core dumped)

Explaination: In the RUN2 you see the bytes read are coming from RUN1 which is wrong.

Output of the program by std file system:
read fd : 4, write fd: 3
bytes written 1024, bytes read 1024

*/