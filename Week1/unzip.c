


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
