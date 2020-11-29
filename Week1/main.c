
#include "definitions.h"
#include "zip.c"
#include "unzip.c"
#include "huffman.c"
#include "runlength.c"

int dir(char* name){
    // returns 1 if the name is a directory, else returns 0
  
    struct stat this_file; 
    if(stat(name,&this_file)==-1){
        perror("Error: ");
        return -1;
    }
    else{
        if(S_ISDIR(this_file.st_mode)){
            return 1;
        
        }
        else{
            return 0;
        }
    }
}

int main(int argc, char* argv[])
{
    
    
    if(strcmp(argv[1],"zip")==0)
    {
        if(strcmp(argv[2],"1")==0)//huffman=1, runlength=2
        {
            for(int i=3;i<argc;i++)
            {   
                if(dir(argv[i])==0)
                {
                    FILE *ptr=fopen(argv[i],"r");
                    char newf_name[400]="henc_";
                    strcat(newf_name,argv[i]);
                    FILE *newf=fopen(&newf_name[0],"w");
                    if(ptr==NULL)
                    {
                        printf("error\n");
                        continue;
                    }
                    char fname[400]={0};
                    strcat(&fname[0],argv[i]);
                    zip(ptr,newf,&fname[0]);
                } 
                else if(dir(argv[i])==1)
                {
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"zipped");
                    mkdir(dirname,0777);
                    
                    while ((d = readdir(dp))) 
                    {
                        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                        char filename[1024]=" ";
                        strcpy(filename,argv[i]);
                        strcat(filename,"/");   
                        strcat(filename,d->d_name);
                        FILE *ptr=fopen(filename,"r");
                        char newf_name[400]=" ";
                        strcpy(newf_name,dirname);
                        strcat(newf_name,"/");
                        strcat(newf_name,"henc_");
                        strcat(newf_name,d->d_name);
                        FILE *newf=fopen(newf_name,"w");
                        if(ptr==NULL)
                        {
                            printf("error\n");
                            continue;
                        }
                        
                        
                        zip(ptr,newf,d->d_name);
                        fclose(ptr);
                        fclose(newf);
                    }
                    closedir(dp);
                }
            }
        }

        else if(strcmp(argv[2],"2")==0)
        {
            char line[1000][1000];
            FILE *file; 
            FILE *fp;
            int number = argc;
            int j=0;
    
            for(int i=3;i<number;i++)
            {   
                j=0;
                if(dir(argv[i])==1)
                {
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"zipped");
                    mkdir(dirname,0777);
          
                    while ((d = readdir(dp)))
                    {
                        char filename[1024]=" ";
                        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                        strcpy(filename,argv[i]);
                        strcat(filename,"/");   
                        strcat(filename,d->d_name);
                        encode(filename,dirname,d->d_name);
                    }
                    closedir(dp);
                }
                else
                {
                    file = fopen(argv[i],"r"); // opens the file in read only mode
                    char a[100];
                    strcpy(a,argv[i]);
                    char b[12]= "zip";
                    strcat(a,b);
                    fp = fopen (a, "w+");
                    if(file ==NULL) 
                        perror("Error: "); // gives an error if the file cannot be opened
                    else
                    {
                        while(fgets(line[j], 1000, file)) 
	                    {
                            j++;
                        }
                        for(int k=0;k<j;k++)
                        {
                            int p=0;
                            while(line[k][p]!=0)
                            {
		                        char ch  = line[k][p];
                                int count = 1;
		                        while(line[k][p] == line[k][p + 1] && line[k][p+1]!=0 && count<9)
			                        count++, p++;
		                        if(count>2)
                                {
                                    if(p==0)
                                    {
                                        fprintf(fp,"%d",count);
                                    }
                                    else
                                    {
                                        fprintf(fp,"%d",count);
                                    }
                                    int extra = 17;			
                                    char waste=extra;
                                    fprintf(fp,"%c",waste);
                                    fprintf(fp,"%c",ch);
                                }
                                else
                                {
                                    for(int i=0;i<count;i++)
                                    {
                                        fprintf(fp,"%c",ch);
                                    }	
                                }
                                p++;
                            }

                        }
        
                    }
                    fclose(fp);
                    fclose(file);
                }

            }
        }
    }
    else if(strcmp(argv[1],"unzip")==0)
    {   
        if(strcmp(argv[2],"1")==0)
        {
            for(int i=3;i<argc;i++)
            {
                if(dir(argv[i])==0)
                {
                    char storage[400]="estore_";
                    for(int k=5;k<strlen(argv[i]);k++)
                    {
                        storage[2+k]=*(argv[i]+k);
                    }
                    FILE *ef=fopen(&storage[0],"r");
                    FILE *curr=fopen(argv[i],"r");
                    char currfname[400]={0};
                    strcat(&currfname[0],argv[i]);
                    unzip(ef,curr,&currfname[0]);
                }
                else if(dir(argv[i])==1)
                {
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"_unzipped");
                    mkdir(dirname,0777);
                    
                    while ((d = readdir(dp))) 
                    {
                        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                        char filename[1024]=" ";
                        strcpy(filename,argv[i]);
                        strcat(filename,"/");   
                        strcat(filename,d->d_name);
                        FILE *ptr=fopen(filename,"r");
                        char decoded[1024] = "";
                        strcpy(decoded,dirname);
                        strcat(decoded,"/");
                        strcat(decoded,d->d_name);
                        char storage[400]="estore_";
                        for(int k=5;k<strlen(d->d_name);k++)
                        {
                            storage[2+k]=*(d->d_name+k);
                        }
                        FILE *ef=fopen(&storage[0],"r");
                        
                        if(ptr==NULL)
                        {
                            printf("error\n");
                            continue;
                        }
                        
                        
                        unzip(ef,ptr,decoded);
                     
                        
                    }
                    closedir(dp);
                }
            }
        }
        else if(strcmp(argv[2],"2")==0)
        {
     
            char line[1000][1000];
            FILE *file; 
            FILE *fp;
            int number = argc;
            int j=0;
    
            for(int i=3;i<number;i++)
            {   j=0;
                if(dir(argv[i])==1)
                {
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"-unzipped");
                    mkdir(dirname,0777);
                    while ((d = readdir(dp))) 
                    {
                        char filename[1024]=" ";
                        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                        strcpy(filename,argv[i]);
                        strcat(filename,"/");   
                        strcat(filename,d->d_name);
                        
                        decode(filename,dirname,d->d_name);
                
                    }
                    closedir(dp);
                }
                else
                {
                    file = fopen(argv[i],"r"); // opens the file in read only mode
                    char a[100]="";
                    strcpy(a,argv[i]);
                    char b[15]= "-unzipped";
                    strcat(a,b);
                    fp = fopen (a, "w+");
                    if(file ==NULL) 
                        perror("Error: "); // gives an error if the file cannot be opened
            
                    else
                    {
                        while(fgets(line[j], 1000, file)) 
                        {
                            j++;    
                        }
	
                        for(int k=0;k<j;k++)
                        {
        
                            int p=0;
                            // char s[10];
                            while(line[k][p]!=0)
                            {
                                char ch = line[k][p];
                                int count = ch - '0';
                                if(count>0 && count<=9)
                                {
                                    char txt = line[k][p+1];
                                    int extra = 17;			
                                    char waste=extra;
                                    if(txt==waste)
                                    {
                                        p=p+2;
                                        char text = line[k][p];
                                        for(int h=0;h<count;h++)
                                        {
                                            fprintf(fp,"%c",text);	
                                        }
                                    }
                                    else
                                    {
                                        fprintf(fp,"%c",ch);		
                                    }
                                }
                                else
                                {
                                    fprintf(fp,"%c",ch);	
                                }
                                p=p+1;
                            }

                        }
        
                    }
                    fclose(fp);
                    fclose(file);
                }
            }
        }
    }   
}

