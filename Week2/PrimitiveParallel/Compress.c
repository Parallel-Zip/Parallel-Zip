

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