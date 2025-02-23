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

// This program describes attributes not getting properly set by the block cache mode when they were sent back to the FUSE library.
// let "c1" is our mountpoint
// The test demonstatres that blobfuse dont keep track of the attributes properly when they get changed.
// If you want to simulate the test, Set the timouts for libfuse in the blobfuse config to 0s and disable writeback caching inside 
// the system by passing the option --disable-writeback-cache as an option while mount.
// The reason for turning off the writeback cache mechanism is the fuse doesn't use the size parameter that was passed by blobfuse 
// When this is writeback cache is used.
int main() {
    int writeFD;
    char* buffer = get_rand_data_buf(); // intialising the buffer with some garbage data
    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c1/foo", O_WRONLY| O_TRUNC | O_CREAT, 0777);
    // Write to the file
    bytesWritten = write(writeFD, buffer, 1024);
    if (bytesWritten != -1){
        printf("Write is success, bytes written: %d\n", bytesWritten);
    }

    struct stat st;
    if (stat("c1/foo", &st) == 0) {
        printf("Size of the file: %ld bytes\n", st.st_size);
        // prints: Size of the file: 0 bytes
        assert(st.st_size == 1024);
        // The above assertion would fail
        // The attribute must not return 0 bytes due to the following reasons:
        // 1. we returned success for writing data to the file and now we are saying the size of the file changed to zero 
        // 2. The following reads by another handles would fail due to the size of the file returned is zero.
    } else {
        perror("stat failed");
    }
    close(writeFD);
    // In the end the size of the file is 1024
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
Write is success, bytes written: 1024
Size of the file: 0 bytes
dc6: dc6.c:38: main: Assertion `st.st_size == 1024' failed.
Aborted (core dumped)

Output of the program by std file system:
Write is success, bytes written: 1024
Size of the file: 1024 bytes

*/