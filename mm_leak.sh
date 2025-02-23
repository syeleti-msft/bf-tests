#!/bin/bash

# Directory to create files in
DIR="c1"
# Number of files to create
NUM_FILES=1000
# Size of each file in KB
FILE_SIZE=1024

# Create files
for i in $(seq 1 $NUM_FILES); do
    # Create a file with the specified size
    dd if=/dev/zero of=file_$i bs=1K count=$FILE_SIZE
done

# Copy the files to mountpoint
cp file_* c1/

# Get the physical memory used by the blobfuse process.
sudo pmap -x `pidof blobfuse2`
# See the Output, It uses the entire memory it was given in the config(i.e., 4096MB) 
# But blobfuse is currently idle not having any files happened. The convention is when
# you dont use memory you give back to the system. but here it locks the memory to the 
# process until blobfuse terminates.


# delete the files in local and system
rm file_*
rm c1/file_*

: '
Output of the Script: 

1917735:   blobfuse2 mount /home/fantom/mycontainer/c1 --foreground
Address           Kbytes     RSS   Dirty Mode  Mapping
0000000000400000    3260    1408       0 r---- blobfuse2
000000000072f000    7812    6996       0 r-x-- blobfuse2
0000000000ed0000    8320    7484       0 r---- blobfuse2
00000000016f1000       4       4       4 r---- blobfuse2
00000000016f2000     404     312     120 rw--- blobfuse2
0000000001757000     204     100     100 rw---   [ anon ]
000000002a220000     132      16      16 rw---   [ anon ]
000000c000000000   16384   13816   13816 rw---   [ anon ]
000000c001000000   49152       0       0 -----   [ anon ]
00007eb5a4000000     132       8       8 rw---   [ anon ]
00007eb5a4021000   65404       0       0 -----   [ anon ]
00007eb5ac000000     132       4       4 rw---   [ anon ]
00007eb5ac021000   65404       0       0 -----   [ anon ]
00007eb5b0000000     132       4       4 rw---   [ anon ]
00007eb5b0021000   65404       0       0 -----   [ anon ]
00007eb5b8000000     132       8       8 rw---   [ anon ]
00007eb5b8021000   65404       0       0 -----   [ anon ]
00007eb5bc000000     132       8       8 rw---   [ anon ]
00007eb5bc021000   65404       0       0 -----   [ anon ]
00007eb5c2200000       4       0       0 -----   [ anon ]
00007eb5c2201000    8192       8       8 rw---   [ anon ]
00007eb5c2c00000       4       0       0 -----   [ anon ]
00007eb5c2c01000    8192       8       8 rw---   [ anon ]
00007eb5c3600000       4       0       0 -----   [ anon ]
00007eb5c3601000    8192       8       8 rw---   [ anon ]
00007eb5c4000000     132       8       8 rw---   [ anon ]
00007eb5c4021000   65404       0       0 -----   [ anon ]
00007eb5c8200000       4       0       0 -----   [ anon ]
00007eb5c8201000    8192      12      12 rw---   [ anon ]
00007eb5c8c00000       4       0       0 -----   [ anon ]
00007eb5c8c01000    8192      12      12 rw---   [ anon ]
00007eb5c9600000       4       0       0 -----   [ anon ]
00007eb5c9601000    8192      12      12 rw---   [ anon ]
00007eb5ca000000 4030464 4022272 4022272 rw---   [ anon ]
00007eb6c0000000     136     136     136 rw---   [ anon ]
00007eb6c0022000   65400       0       0 -----   [ anon ]
00007eb6c4800000   57344   57344   57344 rw---   [ anon ]
00007eb6c8000000     132       4       4 rw---   [ anon ]
00007eb6c8021000   65404       0       0 -----   [ anon ]
00007eb6cc000000     132       4       4 rw---   [ anon ]
00007eb6cc021000   65404       0       0 -----   [ anon ]
00007eb6d0800000   57344   57344   57344 rw---   [ anon ]
00007eb6d4000000     132       4       4 rw---   [ anon ]
00007eb6d4021000   65404       0       0 -----   [ anon ]
00007eb6d8000000     132       4       4 rw---   [ anon ]
00007eb6d8021000   65404       0       0 -----   [ anon ]
00007eb6dc600000   49152   49152   49152 rw---   [ anon ]
00007eb6df600000       4       0       0 -----   [ anon ]
00007eb6df601000    8192      16      16 rw---   [ anon ]
00007eb6dfefe000    1032    1028    1028 rw---   [ anon ]
00007eb6e0000000     132       4       4 rw---   [ anon ]
00007eb6e0021000   65404       0       0 -----   [ anon ]
00007eb6e40fe000    1032    1028    1028 rw---   [ anon ]
00007eb6e4200000       4       0       0 -----   [ anon ]
00007eb6e4201000    8192       8       8 rw---   [ anon ]
00007eb6e4afe000    1032    1028    1028 rw---   [ anon ]
00007eb6e4c00000       4       0       0 -----   [ anon ]
00007eb6e4c01000    8192       8       8 rw---   [ anon ]
00007eb6e54fe000    1032    1028    1028 rw---   [ anon ]
00007eb6e5600000       4       0       0 -----   [ anon ]
00007eb6e5601000    8192       8       8 rw---   [ anon ]
00007eb6e5ec0000    1280    1012    1012 rw---   [ anon ]
00007eb6e6000000       4       0       0 -----   [ anon ]
00007eb6e6001000    8192       8       8 rw---   [ anon ]
00007eb6e6820000    1920    1652    1652 rw---   [ anon ]
00007eb6e6a00000       4       0       0 -----   [ anon ]
00007eb6e6a01000    8192       8       8 rw---   [ anon ]
00007eb6e721c000     912     804     804 rw---   [ anon ]
00007eb6e7300000   33792       8       8 rw---   [ anon ]
00007eb6e9400000  263680       0       0 -----   [ anon ]
00007eb6f9580000       4       4       4 rw---   [ anon ]
00007eb6f9581000  524284       0       0 -----   [ anon ]
00007eb719580000       4       4       4 rw---   [ anon ]
00007eb719581000  293564       0       0 -----   [ anon ]
00007eb72b430000       4       4       4 rw---   [ anon ]
00007eb72b431000   36692       0       0 -----   [ anon ]
00007eb72d806000       4       4       4 rw---   [ anon ]
00007eb72d807000    4068       0       0 -----   [ anon ]
00007eb72dc00000     160     160       0 r---- libc.so.6
00007eb72dc28000    1620    1300       0 r-x-- libc.so.6
00007eb72ddbd000     352     192       0 r---- libc.so.6
00007eb72de15000       4       0       0 ----- libc.so.6
00007eb72de16000      16      16      16 r---- libc.so.6
00007eb72de1a000       8       8       8 rw--- libc.so.6
00007eb72de1c000      52      24      24 rw---   [ anon ]
00007eb72de29000     304     204     204 rw---   [ anon ]
00007eb72de75000     512       0       0 -----   [ anon ]
00007eb72def5000       4       4       4 rw---   [ anon ]
00007eb72def6000     508       0       0 -----   [ anon ]
00007eb72df75000     396      56      56 rw---   [ anon ]
00007eb72dfd8000      32      32       0 r---- libfuse3.so.3.16.2
00007eb72dfe0000     116     116       0 r-x-- libfuse3.so.3.16.2
00007eb72dffd000      40      12       0 r---- libfuse3.so.3.16.2
00007eb72e007000       4       0       0 ----- libfuse3.so.3.16.2
00007eb72e008000      72      52      16 r---- libfuse3.so.3.16.2
00007eb72e01a000       4       4       4 rw--- libfuse3.so.3.16.2
00007eb72e01b000      36      36      36 rw---   [ anon ]
00007eb72e024000       8       8       0 r---- ld-linux-x86-64.so.2
00007eb72e026000     168     168       0 r-x-- ld-linux-x86-64.so.2
00007eb72e050000      44      44       0 r---- ld-linux-x86-64.so.2
00007eb72e05b000       4       4       4 rw---   [ anon ]
00007eb72e05c000       8       8       8 r---- ld-linux-x86-64.so.2
00007eb72e05e000       8       8       8 rw--- ld-linux-x86-64.so.2
00007ffeb2b9f000     132      16      16 rw---   [ stack ]
00007ffeb2bd8000      16       0       0 r----   [ anon ]
00007ffeb2bdc000       8       4       0 r-x--   [ anon ]
ffffffffff600000       4       0       0 --x--   [ anon ]
---------------- ------- ------- -------
total kB         6333736 4226640 4208488

It uses the entire 4GB of the VM and it never frees this memory 
even when the blobfuse is idle.
'