#include "definitions.h"
#include "Compress.c"
#include "Huffman_Coding.c"


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