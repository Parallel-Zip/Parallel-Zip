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

char **filenames;

struct arguments
{
    int argc;
    char **argv;
};

struct arguments * init_arg()
{
    struct arguments *temp=malloc(sizeof(struct arguments));
    temp->argc=0;
    temp->argv=NULL;
    return temp;
}

//structures and functions for zip
struct node 
{
    char identity; 
    int flag;
    int num;
    struct node* left;
    struct node* right;
};

struct node* init()
{
    struct node* ptr=malloc(sizeof(struct node));
    ptr->identity='a';
    ptr->flag=0;
    ptr->num=0;
    ptr->left=NULL;
    ptr->right=NULL;
    return ptr;
}

struct bufferobj
{
    char *pageinram;
    int pagenum;
    int filenum;
    int flag;
    int lastpagesize;
};

struct compobj
{
    char *data;
    int size;

};

struct compobj *zip(struct bufferobj *temp);

struct bufferobj * init_buff()
{
    struct bufferobj *temp= malloc(sizeof(struct bufferobj));
    temp->filenum=0;
    temp->pagenum=0;
    temp->pageinram=NULL;
    temp->flag=0;
    temp->lastpagesize=0;
    return temp;
}

struct compobj* init_comp()
{
    struct compobj *temp=malloc(sizeof(struct compobj));
    temp->data=NULL;
    temp->size=0;
    return temp;
}


void insert_hp(struct node* y, struct node  **arr, int *curr);
void remove_min_hp(struct node  **arr, int* curr);
struct node* make_hf_tree(struct node **arr, int *curr);
void generate_encodings(struct node *node, int** encoding, int str[100], int i);
void make_heap(struct node **arr,int* curr, int *freq);
void read_file(int* freq, char *arr, int pagesize);
struct compobj* compress(int **encodings,struct bufferobj *temp);
void store_encodings( int **encodings, struct bufferobj *temp, int *freq);



//shared resources

    //semaphores
sem_t empty,full,mutex;
    //array to store number of pages in each file
int *pagecnt;
int pagesize=10000000;
int qsize=0,producerptr=0,consumerptr=0; //will tell which location in the buffer should a producer add to to or a consumer consume from
struct bufferobj *buffer[20];  
int done=0;//setting this to 1 will tell consumers that producer is done producing


//array to store compressed contents of each file
//compressed[i] will contain pointer to compressed object corresponding to 1st page of ith file
struct compobj *compressed[100];

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
    consumerptr=(consumerptr+1)%20;
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

        //for the ith file, allocate memory units equal to the # of pages in that file, each unit being of the size of a compressed object
        compressed[i-2]=malloc(sizeof(struct compobj)*pagecnt[i-2]);

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
        *(&compressed[temp->filenum]+temp->pagenum)=pagedone; 
        printf("%d %d\n",temp->filenum+2,temp->pagenum);
        // printf("%s\n",pagedone->data);
    }
    return NULL;
}


int main(int argc, char* argv[])
{
    
    struct timeval start_time, end_time;
    

    filenames=argv;
    // printf("%s",filenames[1]);
    //initialize semaphores
    sem_init(&empty,0,20);//initialized to 20 so that when we run the producer first, it does not sleep
    sem_init(&full,0,0);//initialized to 0 so that even if a consumer thread runs before the producer, it will be put to sleep
    sem_init(&mutex,0,1);//only one thread should be able to acquire the lock at a time, hence initialized to a 1

    //create a thread for producer
    pthread_t p;
    struct arguments *ptr=init_arg();
    ptr->argc=argc;
    ptr->argv=argv;
    gettimeofday(&start_time, NULL);
    pthread_create(&p,NULL,producer,(void *)ptr);

    //create threads for consumers
    int n_threads=get_nprocs();
    pthread_t c[n_threads];
    for(int i=0;i<n_threads;i++)
    {
        pthread_create(&c[i],NULL,consumer,NULL);
    }

    //join all the threads
        //consumer
    for(int i=0;i<n_threads;i++)
    {
        pthread_join(c[i],NULL);
    }
        //producer
    pthread_join(p,NULL);
    gettimeofday(&end_time, NULL);
    long diff=(end_time.tv_sec-start_time.tv_sec)*1000000 + (end_time.tv_usec-start_time.tv_usec);
    printf("time: %f seconds\n",diff*0.000001);

    return 0;
}




//functions for zipping



void insert_hp(struct node *y,struct node  **arr, int *curr)
{
    arr[*curr]=y;
    int curr_copy=*curr;
    while(arr[curr_copy/2]->num > y->num) 
    {
        arr[curr_copy]=arr[curr_copy/2]; 
        curr_copy=curr_copy/2;
    }
    arr[curr_copy]=y;
    *curr+=1;
    return;
}

void remove_min_hp(struct node  **arr, int* curr)
{
    *curr-=1;
    arr[1]=arr[*curr];
    arr[*curr]=init();
    struct node *copy=arr[1];
    int i=1;
    while(i<= *curr/2)
    {
        int i_temp=i;
        if(arr[2*i]!=NULL && arr[2*i]->num >0  && arr[2*i]->num < arr[i]->num)
        {
            arr[i]=arr[2*i];
            i_temp=2*i;
        }
        if(arr[2*i+1]!=NULL && arr[2*i+1]->num >0 && arr[2*i+1]->num < arr[i]->num)
        {
            arr[i]=arr[2*i+1];
            i_temp=2*i+1;
        }
        arr[i_temp]=copy;
        if(i==i_temp)
        {
            break;
        }
        i=i_temp;
    }
    return;
}


struct node* make_hf_tree(struct node **arr, int *curr)
{
    struct node *head;
    while(*curr>2) //while there are 2 or more nodes in the heap
    {
        struct node *temp1,*temp2,*par;
        temp1=arr[1];
        remove_min_hp(&arr[0],curr);
        temp2=arr[1];
        remove_min_hp(&arr[0],curr);
        
        par=init();
        par->num=temp1->num + temp2->num;
        par->left=temp1;par->right=temp2;
        head=par;

        insert_hp(par,&arr[0],curr);

    }

    return head;

}


void generate_encodings(struct node *node, int** encoding, int str[100], int i)
{
    if(node==NULL)return;
    if(node->flag==1)
    {
        int index=node->identity-32;
        if(node->identity=='\n')
        {
            index=96;
        }
        for(int t=0;t<100;t++)
        {
            *(encoding[index]+t)=str[t];
        }
        int k=0;
        return;
    }
    int str1[100];
    int str2[100];
    for(int j=0;j<100;j++)
    {
        str1[j]=str[j];
        str2[j]=str[j];
    }
    str1[i]=0;
    str2[i]=1;
    generate_encodings(node->left,encoding,str1,i+1);
    generate_encodings(node->right,encoding,str2,i+1);
    return;
}

void make_heap(struct node **arr,int* curr, int *freq)
{
    for(int j=0;j<96;j++)
    {
        if(freq[j]>0)
        {
            struct node *temp=init();
            temp->flag=1;
            temp->identity=(char)(j+32);
            temp->num=freq[j];
            insert_hp(temp,&arr[0],curr);
        }
    }
    if(freq[96]>0)
    {
        struct node *temp=init();
        temp->flag=1;
        temp->identity='\n';
        temp->num=freq[96];
        insert_hp(temp,&arr[0],curr);
    }
    return;
}

void read_file(int* freq, char *content, int size)
{
    char x;
    for(int i=0;i<size;i++)
    {
        x=*(content+i);
        if(x=='\n')
        {
            freq[96]+=1;
        }
        else
        {
            int index= (int)x-32;
            freq[index]+=1;
        }
    }
    return;
}


struct compobj* compress(int **encodings,struct bufferobj *temp)
{   
    struct compobj* pagedone=init_comp();
    char x;
    unsigned char y=0;
    int k=0,p=1;
    int size=0;
    if(temp->flag==0)size=pagesize;
    else size=temp->lastpagesize;
    pagedone->data=malloc(sizeof(char)*size);
    int offset=0;
    for(int i=0;i<size;i++)
    {
        x=*(temp->pageinram+i);
        // printf("%c",x);
        if(x=='\n')
        {
            int j=0;
            while(*(encodings[96]+j)>=0)
            {
                y+=*(encodings[96]+j)*p;
                p=p*2;
                k++;
                j++;
                if(k==8)
                {
                    *(pagedone->data+offset)=y;
                    // printf("%c",*(pagedone->data+offset));
                    offset++;
                    // printf("%C ",temp);
                    k=0;p=1;
                    y=0;
                }
            }
        }
        else
        {
            int index=(int)x-32;
            int j=0;
            while(*(encodings[index]+j)>=0)
            {
                y+=*(encodings[index]+j)*p;
                k++;
                p=p*2;
                j++;
                if(k==8)
                {
                    *(pagedone->data+offset)=y;
                    // printf("%c",*(pagedone->data+offset));
                    offset++;
                    // printf("%C ",temp);
                    k=0;p=1;
                    y=0;
                }
            }

        }
        
    }
    if(k>0)
    {
        *(pagedone->data+offset)=y;
        // printf("%c",*(pagedone->data+offset));
        offset++;
    }
    pagedone->size=offset;
    if(realloc(pagedone->data,offset));
    return pagedone;

}

void store_encodings( int **encodings, struct bufferobj *temp, int *freq)
{
    
    char fname[400]="estore_";
    char pgnum[50];
    sprintf(pgnum,"%d",temp->pagenum);
    strcat(fname,&pgnum[0]);
    strcat(fname,"_");
    strcat(fname,filenames[temp->filenum+2]);
    FILE *f=fopen(fname,"w");
    for(int i=0;i<97;i++)
    {
        if(freq[i]>0)
        {
            if(i==96)
            {
                fputc('\n',f);
            }
            else
            {
                fputc(i+32,f);
            }
            int j=0;
            while(*(encodings[i]+j)>=0)
            {
                fputc(*(encodings[i]+j)+'0',f);
                j++;
            }
            fputc('\n',f);
        }
    }
    fclose(f);
}

struct compobj *zip(struct bufferobj *temp)
{   
    struct compobj *pagedone = init_comp();;
    int j;
    char *enc_content;
    int offset=0;
    if(strcmp(filenames[1],"h")==0)
    {   
        int freq[97]={0};
        if(temp->flag==0)
        {  
            read_file(&freq[0],temp->pageinram,pagesize);
        }
        else
        { 
            read_file(&freq[0],temp->pageinram,temp->lastpagesize);

        }
        struct node *arr[200];
        int curr=1;
        arr[0]=init();
        arr[0]->num=-__INT_MAX__;
        make_heap(&arr[0],&curr,&freq[0]);
        struct node *head=make_hf_tree(&arr[0],&curr);
        int* encodings[97];
        for(int k=0;k<97;k++)
        {
            encodings[k]=malloc(sizeof(int)*100);//assuming max length of encoding to be 100
        }
        int str[100];
        for(int p=0;p<100;p++)
        {
            str[p]=-1;
        }
        generate_encodings(head,&encodings[0],str,0);
        pagedone=compress(&encodings[0],temp);
        store_encodings(&encodings[0],temp,&freq[0]);
    }
    else
    {
        int size; 
        if(temp->flag==0)size=pagesize;
        else size=temp->lastpagesize;
        pagedone->data=malloc(sizeof(char)*size);

        if(temp->flag==0)
        {   j=pagesize;
            enc_content = temp->pageinram;
        }
        else
        {   j=temp->lastpagesize;
            enc_content = temp->pageinram;
        }
   
        for(int k=0;k<j;k++)
        {
            unsigned char ch  = *(enc_content+k);
            char count = 49;
            while(*(enc_content+k) == *(enc_content+k+1) && count<57)
                count++, k++;
            if(count>50)
            { 
                *(pagedone->data+offset)=count;
                offset++;
                char extra =130;		
                *(pagedone->data+offset)=extra;
                offset++;
                *(pagedone->data+offset)=ch;
                offset++;
            }
            else
            {
                char charcount = count;
                for(int i=0;i<count-48;i++)
                {   
                    *(pagedone->data+offset)=ch;
                    offset++;
                }	
            }	
                
        }
        pagedone->size=offset;
        if(realloc(pagedone->data,offset));
    }
    return pagedone;

}