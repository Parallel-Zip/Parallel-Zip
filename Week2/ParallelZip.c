#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/stat.h> // for stat
#include<dirent.h> 
#include<pthread.h>
#include<sys/time.h>    




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


void insert_hp(struct node* y, struct node  **arr, int *curr)
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

void read_file(int* freq, FILE *ptr)
{
    char x;
    while((x=fgetc(ptr))!=EOF)
    {
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



void compress(int **encodings, FILE *ptr, FILE * newf)
{   
    rewind(ptr);
    char x;
    unsigned char temp=0;
    int k=0,p=1;
    while((x=fgetc(ptr))!=EOF)
    {
        if(x=='\n')
        {
            int j=0;
            while(*(encodings[96]+j)>=0)
            {
                temp+=*(encodings[96]+j)*p;
                p=p*2;
                k++;
                j++;
                if(k==8)
                {
                    fputc(temp,newf);
                    // printf("%C ",temp);
                    k=0;p=1;
                    temp=0;
                }
            }
        }
        else
        {
            int index=(int)x-32;
            int j=0;
            while(*(encodings[index]+j)>=0)
            {
                temp+=*(encodings[index]+j)*p;
                k++;
                p=p*2;
                j++;
                if(k==8)
                {
                    fputc(temp,newf);
                    // printf("%C ",temp);
                    k=0;p=1;
                    temp=0;
                }
            }

        }
        
    }
    if(k>0)
    {
        fputc(temp,newf);
    }
    return;

}

void store_encodings( int **encodings, char *fname2, int *freq)
{
    char fname[400]="estore_";
    
    strcat(fname,fname2);
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

struct input
{
    FILE *newf;
    FILE *curr;
    char *fname;
};

struct input* init_inp()
{
    struct input *temp=(struct input*)malloc(sizeof(struct input));
    temp->curr=NULL;
    temp->newf=NULL;
    temp->fname=NULL;
    return temp;
}

void *zip(void *arg)
{
    struct input* args=(struct input*)arg;
    FILE *ptr=args->curr;
    FILE *newf=args->newf;
    char *fname=args->fname;
    int freq[97]={0};
    read_file(&freq[0],ptr);//now need to rewind ptr if want to reuse again
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
    compress(&encodings[0],ptr,newf);
    store_encodings(&encodings[0],fname,&freq[0]);
    return NULL;
}



int main(int argc, char* argv[])
{
    struct timeval start_time,end_time;
    gettimeofday(&start_time, NULL);
    pthread_t t[100];
    int t_cnt=0;
    
    if(strcmp(argv[1],"zip")==0)
    {
        for(int i=2;i<argc;i++)
        {   
            FILE *ptr=fopen(argv[i],"r");
            char newf_name[400]="henc_";
            strcat(newf_name,argv[i]);
            FILE *newf=fopen(&newf_name[0],"w");
            if(ptr==NULL || newf==NULL)
            {
                printf("error\n");
                continue;
            }
            char fname[400]={0};
            strcat(&fname[0],argv[i]);
            struct input* inp=init_inp();
            inp->curr=ptr;
            inp->newf=newf;
            inp->fname=argv[i];
            printf("%s\n",inp->fname);
            pthread_create(&t[t_cnt],NULL,zip,(void *)inp);
            t_cnt++;
        } 

        
    }
    for(int h=0;h<=t_cnt-1;h++)
    {
        pthread_join(t[h],NULL);
        printf("done\n");
    }

    gettimeofday(&end_time, NULL);
    long diff=(end_time.tv_sec-start_time.tv_sec)*1000000 + (end_time.tv_usec-start_time.tv_usec);
    printf("%f\n",diff*0.000001);
    return 0;
}
