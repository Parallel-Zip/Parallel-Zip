The aim of the project is to zip the files parallely by building a parallelized version of non concurrent zip.
# Sequential-Zip
First we build a sequential zip using two encoding schemes, Huffmann and Run Length. All the code files related to sequential zip are included in the Week1 folder. 
This was followed by optimizing the encoding schemes to get better performnace in terms of compression time and compressed file size.

![alt text](https://drive.google.com/uc?export=view&id=1BKiDVV7WW3lvMr8sCtwONoIEyRCnLjhc)

# Parallel-Zip
In our first appraoch of adding concurrency to sequential zip, we created number of threads equal to the number of files that are needed to be zipped. Each file is zipped be a single thread parallely. This was better than the Sequential approach but the file with maximum size was deciding the compression time. As the files can be of unequal sizes, work was not equally divided between threads. 

![alt text](https://drive.google.com/uc?export=view&id=1OCBYqB6Gz7mM2IbEgnY6NDqGSLH6Fn_S)

# Better Parallel-Zip
This is a producer consumer based approach. We have considered a single producer and multiple consumers as the work of producer is much less than the work of consumers.
The file is broken into chunks(pages of size 10MB) by the producer and these pages pages are zipped by the consumers.

![alt text](https://drive.google.com/uc?export=view&id=1sJVWQPRXGkyDXMQ6nMWxvZgc9crkPzn4)

# Results
Here is the comaparision between the three approaches. The producer consumer approach of Parallel zip gives much better results and takes comaparatively very less time.

![alt text](https://drive.google.com/uc?export=view&id=1AXYSHSz5EwBV5i4TRODA5u_EhnX_tkCB)

# Usage Instructions
To zip 3 files(file1, file2, file3) using huffmann encoding follow below steps.
</br>
make all
</br>
./main h file1.txt file2.txt file3.txt

Compressed files are stored in "compressed" folder and you can find the test files in the "test_files" folder.
