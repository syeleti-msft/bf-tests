#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

// let "c1" is our mountpoint
int main() {
    int readFD1, readFD2;
    char readbuf[1024];

    // Assume foo is already present in the storage account and containing some data.
    readFD1 = open("c1/foo", O_RDONLY, 0777);
    // Open the same file for reading.
    readFD2 = open("c1/foo", O_RDONLY, 0777);

    // Read from the start of the file with readFD1
    // This will make the blobfuse do a REST call for first 8MB(i.e., block size) data of the blob and put in its blobfuse memory
    int bytesRead = read(readFD1, readbuf, 1024);

    // do Read from the start of the file with readFD2
    // This will make the blobfuse do a another REST call for first 8MB(i.e., block size) data of the same blob again and put in its blobfuse memory
    // Since block cache handles each file description to the file seperatly it will always do a REST call.
    // This can lead to 2 disadvantages:
    // 1. Storing redundant data in the memory(Memory wastage/increase in utilisation)
    // 2. Increase in the REST calls to the service(Increase in cost to customer)
    bytesRead = read(readFD2, readbuf, 1024);

    // Close the file handles
    close(readFD1);
    close(readFD2);

    return EXIT_SUCCESS;
}
