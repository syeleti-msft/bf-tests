#!/bin/bash
# let c1 is our mointpoint where blobfuse is mounted
# This script is demonstrates how data gets corrupted while using "cp" utility.

# Create a 1MB file with random data
dd if=/dev/urandom bs=4096 count=2 of=file_with_holes
dd if=/dev/urandom bs=4096 seek=7 count=0 of=file_with_holes
# That creates for you a file with a nice hole from byte 8192 to byte 28671.

ls -la file_with_holes

# Copy the file to mountpoint 
cp file_with_holes c1/

# Before checking the md5sum lets clear the kernel page cache. to be sure that data is coming from the server.
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"

# Check the MD5sum's of both the files.
md5sum file_with_holes

md5sum c1/file_with_holes

# They dont match as the copied file got corrupted.
: '
Output of the Script: 

2+0 records in
2+0 records out
8192 bytes (8.2 kB, 8.0 KiB) copied, 8.8801e-05 s, 92.3 MB/s
0+0 records in
0+0 records out
0 bytes copied, 5.6301e-05 s, 0.0 kB/s
-rw-rw-r-- 1 fantom fantom 28672 Feb 21 05:20 file_with_holes
eb27fbffc735480babb3a8fe0915a14b  file_with_holes
f6bd95c030219d015a33f559baa71d93  c1/file_with_holes
'