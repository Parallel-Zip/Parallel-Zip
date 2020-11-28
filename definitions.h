
char **filenames;

struct arguments
{
    int argc;
    char **argv;
};
struct node 
{
    char identity; 
    int flag;
    int num;
    struct node* left;
    struct node* right;
};
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

//shared resources

    //semaphores
sem_t empty,full,mutex;
    //array to store number of pages in each file
int *pagecnt;
int pagesize=10000000;
int producerptr=0,consumerptr=0; //will tell which location in the buffer should a producer add to to or a consumer consume from
struct bufferobj *buffer[20];  
_Atomic volatile int done=0, qsize=0;//setting this to 1 will tell consumers that producer is done producing


//array to store compressed contents of each file
//compressed[i] will contain pointer to compressed object corresponding to 1st page of ith file
struct compobj * compressed[100];


void insert_hp(struct node* y, struct node  **arr, int *curr);
void remove_min_hp(struct node  **arr, int* curr);
struct node* make_hf_tree(struct node **arr, int *curr);
void generate_encodings(struct node *node, int** encoding, int str[100], int i);
void make_heap(struct node **arr,int* curr, int *freq);
void read_file(int* freq, char *arr, int pagesize);
struct compobj* compress(int **encodings,struct bufferobj *temp);
void store_encodings( int **encodings, struct bufferobj *temp, int *freq);
void create_compressed_files(struct compobj* compressed[], int n);
struct compobj *zip(struct bufferobj *temp);



void put(struct bufferobj *bfobj);
struct bufferobj* get();
void * producer(void* arg);
void* consumer(void* arg);
struct bufferobj * init_buff();
struct compobj* init_comp();
struct node* init();