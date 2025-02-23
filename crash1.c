#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_FILES 2000
#define BUF_SIZE 4096


// This program demonstrates how mountpoint can get crashed when using blobfuse blockcache.
// The primary reason for it is there is no global buffer eviction policy that was present in blobfuse.
// Therefore buffers are blocked to the handle and not get freed until the user closes the file.
// This program just create the files of size 8M and 2000 files were created but the process gets killed by linux
// as it uses entire system memory available in the VM.
// This is primarily due to the lack of buffer eviction policy on the buffers.
int main() {
    int fds[NUM_FILES];
    char buf[BUF_SIZE];
    int i;

    // Generate files
    for (i = 0; i < NUM_FILES; i++) {
        // Open/Create the file for writing.
        char filename[20];
        snprintf(filename, sizeof(filename), "c2/file_%d", i + 1);
        int writeFD = open(filename, O_WRONLY| O_TRUNC | O_CREAT, 0777);
        // Write 8MB to the file
        for(int i = 0; i < 2048; i++) {
            if (write(writeFD, buf, BUF_SIZE) == -1) {
                perror("Error Writing to the file");
                return 1;
            }
        }
        close(writeFD);
    }

    // Open the files and read them
    for (i = 0; i < NUM_FILES; i++) {
        // Construct the filename (assuming file_i is like "file_1", "file_2", ..., "file_2000")
        char filename[20];
        snprintf(filename, sizeof(filename), "c2/file_%d", i + 1);

        // Open the file
        fds[i] = open(filename, O_RDONLY);
        if (fds[i] == -1) {
            perror("Error opening file");
            return 1;  // Exit if any file can't be opened
        }
                printf("Dowloading the file %s with fd : %d\n", filename,fds[i]);

        // Read the content of the file into the buffer
        for (int i = 0; i < 2048; i++) {
            if (read(fds[i], buf, BUF_SIZE) == -1) {
                perror("Error reading file");
                return 1;  // Exit if reading fails
            }    
        }
    }

    // defer the close of all the files that were opened for reading 
    for (i = 0; i < NUM_FILES; i++) {
        if (close(fds[i]) == -1) {
            perror("Error closing file");
            return 1;  // Exit if closing fails
        }
    }

    return 0;
}
