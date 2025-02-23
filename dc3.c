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

// let "c1" is our mountpoint
// If you want to simulate the test, Set the timouts to zero for attribute cache for FUSE library.
// This program demonstrates data corrupted while expaning the file size using truncate while writing to the file.
int main() {
    int writeFD;
    char* buffer = get_rand_data_buf(); // intialising the buffer with some garbage data
    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c1/foo", O_WRONLY| O_TRUNC | O_CREAT, 0777);
    // Write to the file
    bytesWritten = write(writeFD, buffer, 1024);
    bytesWritten = write(writeFD, buffer, 1024);
    // to this step the size of the file has become 2048
    // Truncate the file to 4096 bytes(i.e., expand the file)
    if (ftruncate(writeFD, 4096) == -1) {
        perror("Truncate failed\n");
    } else {
        printf("Truncate is Success\n");
    }
    // Continue writing to the file
    bytesWritten = write(writeFD, buffer, 1024);
    // Now still the file size is 4096

    // Close the file handles
    close(writeFD);
    // Now when we are closing the file it dont have the context that truncate call has happened.
    // The handle only knows that file has been written 3 times and its file size is 3072 bytes.

    // lets check the size of the file.
    struct stat st;
    if (stat("c1/foo", &st) == 0) {
        printf("Size of the file: %ld bytes\n", st.st_size);
        assert(st.st_size == 4096);
        // The above assertion would fail and the size it prints is 3072.

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
Truncate is Success
Size of the file: 3072 bytes
dc3: dc3.c:50: main: Assertion `st.st_size == 4096' failed.
Aborted (core dumped)

Output of the program by std file system:
Truncate is Success
Size of the file: 4096 bytes

*/