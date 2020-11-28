#include<stdio.h>
#include<stdlib.h>//malloc
#include<sys/stat.h>
#include<assert.h>
#include<sys/mman.h>//mmap
#include<semaphore.h>
#include<pthread.h>
#include<sys/sysinfo.h>//get_nprocs
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<stdatomic.h>
// #include"definitions.h"

//put and get 
void put(struct bufferobj *bfobj)
{
    buffer[producerptr]=bfobj;
    producerptr=(producerptr+1)%20;
    qsize++;
    // printf("%d ",producerptr);
    return;
}

struct bufferobj* get()
{
    struct bufferobj *temp=buffer[consumerptr];
    consumerptr=(consumerptr+1)%20 ;
    qsize--;
    // printf("%d\n",consumerptr);
    return temp;
}

//producer
void * producer(void* arg)
{   
    struct arguments* inp=(struct arguments*)arg;
    int numoffiles=inp->argc-2;//this will be number of files
    char **argv=inp->argv;
    // printf("%d\n",numoffiles);
    pagecnt=malloc(sizeof(int)*numoffiles);//this is an array to store page count of files
    // printf("%d\n",pagecnt[2]);
    for(int i=2;i<numoffiles+2;i++)
    {
        //open the file
        int fd=open(argv[i],O_RDONLY);
        struct stat filestats;

        //stat the file
        if(fstat(fd,&filestats)==-1)
        {
            printf("stat_error\n");
            continue;
        }
        //check if file is empty
        if(filestats.st_size==0)continue;

        //if not, find the number of pages required for the file
        int lastpagesize=0;
        assert(pagesize>0);
        pagecnt[i-2]=filestats.st_size/pagesize;
        //if last page has a size different from pagesize
        if(filestats.st_size%pagesize>0)
        {
            pagecnt[i-2]++;
            lastpagesize=filestats.st_size%pagesize;
        }
        else
        {
            lastpagesize=pagesize;
        }
        // printf("%d\n",pagecnt[i-2]);
        // printf("%d\n",lastpagesize);

        //for the ith file, compressed[i-2] stores a base address of a block of memory that can hold pagecnt[i-2] number of compressed objects 
        compressed[i-2]=malloc(pagecnt[i-2]*sizeof(struct compobj));

        //mapping the entire file into memory
        char* map;
        //mmap returns a virtual address, this will be the base address for the pages allocated to the file in ram
        if((map=mmap(NULL,filestats.st_size,PROT_READ,MAP_SHARED,fd,0))==MAP_FAILED) 
        {
            close(fd);
            printf("Couldn't map %s\n",argv[i-2]);
            continue;
        }

        
        //adding pages in the file into the buffer
        for(int j=0;j<pagecnt[i-2];j++)
        {
            //you don't need to wait to keep a page obj ready
            struct bufferobj *bfobj=init_buff();
            bfobj->filenum=i-2;
            bfobj->pagenum=j;
            bfobj->pageinram=map;
            
            //need the lastpagesize only when the obj corresponds to last page
            if(j==pagecnt[i-2]-1)
            {
                bfobj->flag=1;
                bfobj->lastpagesize=lastpagesize;
            }
            // printf("%d\n",j);
            sem_wait(&empty);//wait until there are empty slots
            sem_wait(&mutex);//acquire the lock to get a unique buffer object
            put(bfobj);
            // printf("%d %d\n",bfobj->filenum,bfobj->pagenum);
            sem_post(&mutex);
            sem_post(&full);
            map+=pagesize;//now map has the virtual address of the next page
        }

        //once all pages of that file have been added to the buffer, close the file 
        close(fd);
        
    }
    //after producer is done adding pages into buffer
    done=1;

    //wake up all sleeping threads so that they can return from function
    for(int k=0;k<10;k++)
    {
        sem_post(&full);
    }
    return NULL;
}

//consumer
void* consumer(void* arg)
{
    while(!done || qsize)
    {
        sem_wait(&full);//are there any full slots
        sem_wait(&mutex); 
        struct bufferobj *temp= get();
        // printf("%d %d\n",temp->filenum,temp->pagenum);
        sem_post(&mutex); 
        sem_post(&empty);//wake up producer 
        struct compobj *pagedone=zip(temp);
        (compressed[temp->filenum]+temp->pagenum)->data=pagedone->data;
        (compressed[temp->filenum]+temp->pagenum)->size=pagedone->size;
        // *(&compressed[temp->filenum]+temp->pagenum)=pagedone; 
        printf("%d %d\n",temp->filenum+2,temp->pagenum);
        // printf("%s\n",pagedone->data);
    }
    return NULL;
}