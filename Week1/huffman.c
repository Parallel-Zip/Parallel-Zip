
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
        // int k=0;
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