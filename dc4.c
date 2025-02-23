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
// This program demonstrates data corruption when shrinking the file size using truncate while writing to the file.
int main() {
    int writeFD;
    char* buffer = get_rand_data_buf(); // intialising the buffer with some garbage data
    ssize_t bytesRead, bytesWritten;

    // Open/Create the file for writing.
    writeFD = open("c1/foo", O_RDWR | O_TRUNC | O_CREAT, 0777);
    // Write to the file
    bytesWritten = write(writeFD, buffer, 1024);
    bytesWritten = write(writeFD, buffer, 1024);
    // to this step the size of the file has become 2048
    // Truncate the file to 1024 bytes(i.e., expand the file)
    if (ftruncate(writeFD, 1024) == -1) {
        perror("Truncate failed\n");
    } else {
        printf("Truncate is Success\n");
    }
    // Continue writing to the file
    bytesWritten = write(writeFD, buffer, 1024);
    // Now still the file size is 3072

    // Close the file handles
    close(writeFD);
    // Now when we are closing the file it dont have the context that truncate call has happened.
    // The handle only knows that file has been written 3 times and its file size is 3072 bytes.
    // Now the file is dddddddd...dddddddd....dddddddd...  (d is some byte that is present in buffer)  
    //                           ^          ^           ^
    //                           |          |           |
    //                          1024      2048        3072 
    // Expected file   dddddddd...\0\0\0\0\0...ddddddd....
    // But after truncate has call got success there was a hole created in middle of the file that is
    // you see in the above file representation.
    // The hole was not created in the file created by blobfuse hence file got corrupted.
    // lets check the size of the file.
    struct stat st;
    if (stat("c1/foo", &st) == 0) {
        printf("Size of the file: %ld bytes\n", st.st_size);
        // The following assertion would success but the file got corrupted
        assert(st.st_size == 3072);
        // Do the assertion for file corruption
        char expecteddata[3072];
        memcpy(expecteddata, buffer, BUF_SIZE);
        memset(expecteddata + 1024, '\0', BUF_SIZE);
        memcpy(expecteddata + 2048, buffer, BUF_SIZE);

        int readFD = open("c1/foo", O_RDONLY, 0777);
        char filedata[3072];
        bytesRead = read(readFD, filedata, 3072);
        assert(bytesRead == 3072);
        // The following assertion would fail in the blobfuse corrupting the data.
        assert(memcmp(expecteddata, filedata, 3072) == 0);
        
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
dc4: dc4.c:71: main: Assertion `memcmp(expecteddata, filedata, 3072) == 0' failed.
Aborted (core dumped)

Output of the program by std file system:
Truncate is Success
Size of the file: 3072 bytes

*/