

void encode(char* filename,char* dirname,char* file_1)
{
    if(dir(filename)==0)
    {
     
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

        if(file ==NULL)perror("Error: "); // gives an error if the file cannot be opened
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
                    {
                        count++, p++;
                    } 
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

void decode(char* filename,char* dirname,char* file_1)
{
    if(dir(filename)==0)
    {
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
        {
            perror("Error: "); // gives an error if the file cannot be opened
        }
        else
        {
            
            while(fgets(line[j],1000, file)) 
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
