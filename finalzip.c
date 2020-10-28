#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/stat.h> // for stat
#include <dirent.h> 

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

void encode(char* filename,char* dirname,char* file_1){
    if(dir(filename)==0){
     
        char line[1000][1000];
        FILE *file; 
        FILE *fp;
        int j=0;
       
        file = fopen(filename,"r"); // opens the file in read only mode
        char a[100];
        strcpy(a,dirname);
        strcat(a,"/");   
        strcat(a,file_1);
        
        char b[12]= "zip";
        strcat(a,b);
        fp = fopen (a, "w+");

        if(file ==NULL) 
            perror("Error: "); // gives an error if the file cannot be opened
            
        else{
            
            while(fgets(line[j], 1000, file)) 
	    {
                j++;
                
        }
                    for(int k=0;k<j;k++){
      
                    int p=0;
                    while(line[k][p]!=0){
		            char ch  = line[k][p];
		 
                    int count = 1;
		            while(line[k][p] == line[k][p + 1] && line[k][p+1]!=0 && count<9)
			            count++, p++;
		        if(count>2){
                  if(p==0){
			        fprintf(fp,"%d",count);
			        }
			    else{
		        	fprintf(fp,"%d",count);}
		        int extra = 17;			
		        char waste=extra;
		        fprintf(fp,"%c",waste);
		        fprintf(fp,"%c",ch);
		        }
		        else{
		        for(int i=0;i<count;i++){
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

void decode(char* filename,char* dirname,char* file_1){
    if(dir(filename)==0){
        
    char line[1000][1000];
    FILE *file; 
    FILE *fp;
    int j=0;
    file = fopen(filename,"r"); // opens the file in read only mode
        char a[100];
        strcpy(a,dirname);
        strcat(a,"/");   
        strcat(a,file_1);
        
        char b[15]= "-unzipped";
        strcat(a,b);
        fp = fopen (a, "w+");
    if(file ==NULL) 
        perror("Error: "); // gives an error if the file cannot be opened
            
    else{
            
        while(fgets(line[j],1000, file)) 
	    {
            j++;
                
        }
	
            for(int k=0;k<j;k++){
      
            int p=0;
	    char s[10];
            while(line[k][p]!=0){
		char ch = line[k][p];
		int count = ch - '0';
	
		if(count>0 && count<=9){
			char txt = line[k][p+1];
			int extra = 17;			
			char waste=extra;
		  if(txt==waste){
			p=p+2;
			char text = line[k][p];
		    for(int h=0;h<count;h++){
			fprintf(fp,"%c",text);
            	
		    }
		  }
		  else{
              
			fprintf(fp,"%c",ch);		
		  }
		}
		else{
			fprintf(fp,"%c",ch);	
		}
                p=p+1;

            }

        }
        
        }
        fclose(fp);
        fclose(file);
}}


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

//functions related to file handling
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
        // printf("%C ",temp);
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


void zip(FILE *ptr, FILE *newf, char* fname)
{
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
    return;
}

void extract_encodings_from_storage(FILE *ef,char **encds)
{
    for(int i=0;i<97;i++)
    {
        encds[i]=malloc(100);
    }
    char x;
    while((x=fgetc(ef))!=EOF)
    {
        if(x=='\n')
        {
            int j=0;
            while ((x=fgetc(ef))!='\n')
            {
                *(encds[96]+j)=x;
                j++;
            }
        }
        else
        {
            int index=x-32;
            int j=0;
            while ((x=fgetc(ef))!='\n')
            {
                *(encds[index]+j)=x;
                j++;
            }
        }   
    }
    return;
}

void convert_back_to_binary(FILE *tf, char *currfname)
{   
    FILE *newptr=fopen(currfname,"w");
    int xyz;
    rewind(tf);
    fseek(tf,0,SEEK_END);
    int fsize=ftell(tf);
    fseek(tf,0,SEEK_SET);
    int count=0;
    while((xyz=fgetc(tf))!=EOF && count<fsize-1) //>=0
    {
        int temp=xyz;
        int cnt=0;
        while(temp>0)
        {
            fputc(temp%2 +'0',newptr);
            temp/=2;
            cnt++;
        }
        while(cnt<8)
        {
            fputc('0',newptr);
            cnt++;
        }
        count++;
    }
    xyz=fgetc(tf);
    int temp=xyz;
    while(temp>0)
    {
        fputc(temp%2 +'0',newptr);
        temp/=2;
    }
    fclose(newptr);
    return;
}


void decrypt(FILE *tf, char **encds, char *currfname)
{   
    rewind(tf);
    FILE *p=fopen(currfname,"r");
    int r=0;
    int c;
    int signal=0;
    int flag[97]={0};
    while((c=fgetc(p))!=EOF )
    {  
        char c2=(char)c;
        for(int i=0;i<97;i++)
        {
            if(c2==*(encds[i]+r))
            {  
                if(flag[i]==0 && r==strlen(encds[i])-1)
                {
                    if(i==96)
                    {
                        fputc(10,tf);
                    }
                    else
                    {
                        fputc(i+32,tf);
                    }
                    r=0;
                    for(int h=0;h<97;h++)
                    {
                        flag[h]=0;
                    }
                    signal=1;
                    break;
                }
                
            }
            else
            {
                flag[i]=1;
            }
        }
        if(signal==1)
        {
            r=0;
            signal=0;
        }
        else
        {
            r++;
        }
        
    }
    fclose(p);
}


void unzip(FILE *ef, FILE *curr, char* currfname)
{   
    char *encds[97];

    extract_encodings_from_storage(ef,&encds[0]);

    FILE *tf=tmpfile();
    //store the contents of original file into temporary file with fd 'tf'
    int y; 
    while((y=fgetc(curr))!=EOF)
    { 
        if(y<0)y=256-abs(y);
        fputc(y,tf);
    }
   
    fclose(curr);

    // using tf, convert back original file into binary
    convert_back_to_binary(tf, currfname);
    
    //decrypt the binary file using encodings stored in encds
    decrypt(tf,&encds[0],currfname);

    //final contents are in tf, copy them to original file
    FILE *final=fopen(currfname,"w");
    rewind(tf);
    int xyz;
    while((xyz=fgetc(tf))!=EOF)
    {
        char x=(char)xyz;
        fputc(x,final);
        // printf("%c",x);
    }
    
    return ;
}



int main(int argc, char* argv[])
{
    
    
    if(strcmp(argv[1],"zip")==0)
    {
        if(strcmp(argv[2],"1")==0)//huffman=1, runlength=2
        {
            for(int i=3;i<argc;i++)
            {   if(dir(argv[i])==0){
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
                else if(dir(argv[i])==1){
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"zipped");
                    mkdir(dirname,0777);
                    
                    while ((d = readdir(dp))) {
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

        else if(strcmp(argv[2],"2")==0){
            char line[1000][1000];
            FILE *file; 
            FILE *fp;
            int number = argc;
            int j=0;
    
            for(int i=3;i<number;i++)
            {   j=0;
                if(dir(argv[i])==1){
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"zipped");
                    mkdir(dirname,0777);
          
                    while ((d = readdir(dp))) {

                        char filename[1024]=" ";
                        if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                        strcpy(filename,argv[i]);
                        strcat(filename,"/");   
                        strcat(filename,d->d_name);
                
                        encode(filename,dirname,d->d_name);
                 }
                    closedir(dp);
                }
                else{
                file = fopen(argv[i],"r"); // opens the file in read only mode
                char a[100];
                strcpy(a,argv[i]);
                char b[12]= "zip";
                strcat(a,b);
                fp = fopen (a, "w+");

                if(file ==NULL) 
                    perror("Error: "); // gives an error if the file cannot be opened
            
                else{
            
                    while(fgets(line[j], 1000, file)) 
	            {
                        j++;
                
                }
                    for(int k=0;k<j;k++){
      
                    int p=0;
                    while(line[k][p]!=0){
		            char ch  = line[k][p];
		 
                    int count = 1;
		            while(line[k][p] == line[k][p + 1] && line[k][p+1]!=0 && count<9)
			            count++, p++;
		        if(count>2){
                  if(p==0){
			        fprintf(fp,"%d",count);
			        }
			    else{
		        	fprintf(fp,"%d",count);}
		        int extra = 17;			
		        char waste=extra;
		        fprintf(fp,"%c",waste);
		        fprintf(fp,"%c",ch);
		        }
		        else{
		        for(int i=0;i<count;i++){
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
    {   if(strcmp(argv[2],"1")==0){

    
        for(int i=3;i<argc;i++)
        {
            if(dir(argv[i])==0){
            
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
            
            else if(dir(argv[i])==1){
                    DIR *dp; // pointer of direcrory stream
                    struct dirent *d; // Pointer for directory entry
                    char dirname[1024]="";
                    dp = opendir(argv[i]); // open directory passed in argument 
                    strcpy(dirname,argv[i]);
                    strcat(dirname,"_unzipped");
                    mkdir(dirname,0777);
                    
                    while ((d = readdir(dp))) {
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
    else if(strcmp(argv[2],"2")==0){
     
    char line[1000][1000];
    FILE *file; 
    FILE *fp;
    int number = argc;
    int j=0;
    
    for(int i=3;i<number;i++)
    {   j=0;
        if(dir(argv[i])==1){
            DIR *dp; // pointer of direcrory stream
            struct dirent *d; // Pointer for directory entry
            char dirname[1024]="";
            dp = opendir(argv[i]); // open directory passed in argument 
            strcpy(dirname,argv[i]);
            strcat(dirname,"-unzipped");
            mkdir(dirname,0777);
          
            while ((d = readdir(dp))) {

                char filename[1024]=" ";
                if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))continue;
                strcpy(filename,argv[i]);
                strcat(filename,"/");   
                strcat(filename,d->d_name);
                
                decode(filename,dirname,d->d_name);
                
            }
            closedir(dp);
        }
        else{
        file = fopen(argv[i],"r"); // opens the file in read only mode
        char a[100]="";
        strcpy(a,argv[i]);
        char b[15]= "-unzipped";
        strcat(a,b);
        fp = fopen (a, "w+");
        if(file ==NULL) 
            perror("Error: "); // gives an error if the file cannot be opened
            
        else{
            
            while(fgets(line[j], 1000, file)) 
	    {
                j++;
                
        }
	
            for(int k=0;k<j;k++){
      
            int p=0;
	    char s[10];
            while(line[k][p]!=0){
		char ch = line[k][p];
		int count = ch - '0';
	
		if(count>0 && count<=9){
			char txt = line[k][p+1];
			int extra = 17;			
			char waste=extra;
		  if(txt==waste){
			p=p+2;
			char text = line[k][p];
		    for(int h=0;h<count;h++){
			fprintf(fp,"%c",text);	
		    }
		  }
		  else{
			fprintf(fp,"%c",ch);		
		  }
		}
		else{
			fprintf(fp,"%c",ch);	
		}
                p=p+1;

            }

        }
        
        }
        fclose(fp);
        fclose(file);
}}
    }

    }
    
}

