### Usage Instructions

To zip 3 files(file1, file2, file3) using huffmann encoding follow below steps.
</br>
make all
</br>
./main h file1.txt file2.txt file3.txt
</br>
### The aim of the project is to zip the files parallely by building a parallelized version of non concurrent zip.
# Sequential-Zip
First we build a sequential zip using two encoding schemes, Huffmann and Run Length. All the code files related to sequential zip are included in the Week1 folder. 
This was followed by optimizing the encoding schemes to get better performnace in terms of compression time and compressed file size.

<img src="https://drive.google.com/uc?export=view&id=1BKiDVV7WW3lvMr8sCtwONoIEyRCnLjhc" >

# Parallel-Zip
In our first appraoch of adding concurrency to sequential zip, we created number of threads equal to the number of files that are needed to be zipped. Each file is zipped be a single thread parallely. This was better than the Sequential approach but the file with maximum size was deciding the compression time. As the files can be of unequal sizes, work was not equally divided between threads. 

<img src="https://drive.google.com/uc?export=view&id=1OCBYqB6Gz7mM2IbEgnY6NDqGSLH6Fn_S" height="200">

# Better Parallel-Zip
This is a producer consumer based approach. We have considered a single producer and multiple consumers as the work of producer is much less than the work of consumers.
The file is broken into chunks(pages of size 10MB) by the producer and these pages pages are zipped by the consumers.

<img src="https://drive.google.com/uc?export=view&id=1sJVWQPRXGkyDXMQ6nMWxvZgc9crkPzn4" height="300" >

# Producer Consumer approach Implementation 
## Finding optimum buffer-size and pagesize

<img src="https://drive.google.com/uc?export=view&id=1mayJU4FXhwkSMCmghPzzBTaHiXLrjuo3" height="300" >
Buffersize of 20 and pagesize of 10 MB is used in the algorithm(extimated from above graphs)

## Varying number of consumer threads

<img src="https://drive.google.com/uc?export=view&id=1dnbvZTK75g4J5syXFcjGx0uf8C10NcC9" height="300" >
The number of threads are decided based on the number of available cores using get_nprocs().

# Results
Here is the comaparision between the three approaches. The producer consumer approach of Parallel zip gives much better results and takes comaparatively very less time.

<img src="https://drive.google.com/uc?export=view&id=1AXYSHSz5EwBV5i4TRODA5u_EhnX_tkCB" height="300" >

## Our approach vs (Pzip and WinZip and 7zip)
Workload: 5KB, 25KB, 25MB
size after compression using:-
1. Pzip implemented by us: 2.4KB, 15KB, 15MB
2. Winzip: 1.25KB, 10.43KB, 308.49KB
3. Zip command in Linux: 1.5KB, 11.3KB, 205.3KB
4. 7zip: 1.44KB, 10.3KB, 14KB

It can be seen that for larger files, their compression algorithm proved to be much better than ours, while there was no observable difference in the time taken by all the tools for zipping
Compressed files are stored in "compressed" folder and you can find the test files in the "test_files" folder.

# Limitations and Scope
Parallel zip has been implemented for text files and in future it can be extentded to zip directories and other formats like .jpeg</br>
Approach can also be extended to unzip files parallely.</br>
Although mmap helps to reduce overhead and equally divide the workload between thread, but it also results in waste of memory because the page size is an interger value.
