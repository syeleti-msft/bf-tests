#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

#define BUF_SIZE 1024
char* get_rand_data_buf();


// let "c1" is our mountpoint
// The test simulate the truncate call causes data corruption in block cache when truncate happened while other file handle is not closed.
// If you want to simulate the test, Set the timouts to zero for attribute cache for FUSE library cozz once truncate 
// call returns success kernel updates its attribute cache with size set in truncate.

int main() {
    int writeFD;
    char* buffer = get_rand_data_buf(); // intialising the buffer with some garbage data
    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c1/foo", O_WRONLY | O_TRUNC | O_CREAT, 0777);
    if (writeFD == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Write at the start of the file
    bytesWritten = write(writeFD, buffer, BUF_SIZE);
    printf("write fd: %d\n", writeFD);
    // Truncate the file to 4096 bytes
    if (ftruncate(writeFD, 4096) == -1) {
        perror("Truncate failed\n");
    } else {
        printf("Truncate is Success\n");
    }
    // The above truncate call go directly to azure storage and modify the file to size 4096.

    // Close the file handles
    close(writeFD);
    // Now when we are closing the file it dont have the context that truncate call happened.
    // The handle only knows that file size is 1024 so it will again modify the filesize to 1024.

    // lets check the size of the file.
    struct stat st;
    if (stat("c1/foo", &st) == 0) {
        printf("Size of the file: %ld bytes\n", st.st_size);
        assert(st.st_size == 4096);
        // The above assertion would fail and the size it prints is 1024.
    } else {
        perror("stat failed");
    }

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
Output of the program by block cache:
write fd: 3
Truncate is Success
Size of the file: 1024 bytes
dc2: dc2.c:51: main: Assertion `st.st_size == 4096' failed.
Aborted (core dumped)

Output of the program by std file system:
write fd: 3
Truncate is Success
Size of the file: 4096 bytes

*/