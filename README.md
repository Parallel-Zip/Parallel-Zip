### Usage Instructions

To zip 3 files(file1, file2, file3) using huffmann encoding follow below steps.
</br>
make all
</br>
./main h file1.txt file2.txt file3.txt
</br>
Compressed files are stored in "compressed" folder and you can find the test files in the "test_files" folder.

### The aim of the project was to zip multiple files (alpha-numeric text) parallely, and identify and resolve the Straggler's problem that occurs when we use multi-threading.
# Sequential-Zip
First we build a sequential zip using two encoding schemes, Huffmann and Run Length. All the code files related to sequential zip are included in the Week1 folder. 
This was followed by optimizing the encoding schemes to get better performnace in terms of compression time and compressed file size.

<img src="https://drive.google.com/uc?export=view&id=1BKiDVV7WW3lvMr8sCtwONoIEyRCnLjhc" >

# Parallel-Zip
In our first appraoch of adding concurrency to sequential zip, we gave one file to each thread. This was better than the Sequential approach but the file with maximum size was deciding the compression time. As the files can be of unequal sizes, work was not equally divided between threads, and so the process did not put equal load on all cores.

<img src="https://drive.google.com/uc?export=view&id=1OCBYqB6Gz7mM2IbEgnY6NDqGSLH6Fn_S" height="200">

# Better Parallel-Zip
This is a producer-consumer based approach. We have considered a single producer and multiple consumers. This is because a producer is only required to create memory mapping for files whereas the consumers are required to compress the files.The file is broken into chunks(pages of size 10MB) by the producer and these pages pages are zipped by the consumers.

<img src="https://drive.google.com/uc?export=view&id=1sJVWQPRXGkyDXMQ6nMWxvZgc9crkPzn4" height="300" >

# Producer Consumer approach Implementation 
## Finding optimum buffersize and pagesize

<img src="https://drive.google.com/uc?export=view&id=1mayJU4FXhwkSMCmghPzzBTaHiXLrjuo3" height="300" >

Inferences
1. Varying only buffer size does not increase or decrease time.
2. Takes more time for very large or very small page size.

## Varying number of consumer threads

<img src="https://drive.google.com/uc?export=view&id=1dnbvZTK75g4J5syXFcjGx0uf8C10NcC9" height="300" >

Inferences
1. Increasing the number of consumer threads to more than number of cores does not decrease time. 


# Results
This is the performance comaparision between the three approaches. The producer consumer approach takes much less time.

<img src="https://drive.google.com/uc?export=view&id=1AXYSHSz5EwBV5i4TRODA5u_EhnX_tkCB" height="300" >

## Our approach vs (WinZip,Zip command Linux,7zip)
Workload: 5KB, 25KB, 25MB
size after compression using:-
1. Pzip implemented by us: 2.4KB, 15KB, 15MB
2. Winzip: 1.25KB, 10.43KB, 308.49KB
3. Zip command in Linux: 1.5KB, 11.3KB, 205.3KB
4. 7zip: 1.44KB, 10.3KB, 14KB

It can be seen that for larger files, their compression algorithm proved to be much better than ours, while there was no observable difference in the time taken by all the tools for zipping.

# Limitations and Scope
1. Parallel zip has been implemented for text files and in future it can be extentded to zip directories and other formats like .jpeg</br>
2. Approach can also be extended to unzip files parallely.</br>
3. Although mmap helps to reduce overhead and equally divide the workload between thread, but it also results in waste of memory because space equal to integral multiple of page size is reserved for each file.
