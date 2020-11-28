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
    pagedone->data=realloc(pagedone->data,offset);
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
                // char charcount = count;
                for(int i=0;i<count-48;i++)
                {   
                    *(pagedone->data+offset)=ch;
                    offset++;
                }	
            }	
                
        }
        pagedone->size=offset;
        pagedone->data=realloc(pagedone->data,offset);
    }
    if(temp->flag==1)munmap(temp->pageinram,temp->lastpagesize);
    else munmap(temp->pageinram,pagesize);
    return pagedone;

}

void create_compressed_files(struct compobj* compressed[], int n)
{
    mkdir("compressed",0777);
    for(int i=2;i<n;i++)
    {
        char * fname=malloc(sizeof(char)*(13+5+strlen(filenames[i])+1));
        strcpy(fname,"");
        strcat(fname,"./compressed/");
        strcat(fname,"henc_");
        strcat(fname,filenames[i]);
        FILE *ptr=fopen(fname,"w");
        int pages=pagecnt[i-2];
        for(int j=0;j<pages;j++)
        {
            struct compobj temp=*(compressed[i-2]+j); //compressed[i-2] stores a compressed obj itself and not a pointer to it
            for(int k=0;k<temp.size;k++)
            {
                fputc(temp.data[k],ptr);
            }
        }
        fclose(ptr);

    }
}