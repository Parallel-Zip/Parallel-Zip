#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/stat.h> // for stat
#include<dirent.h> 
#include<pthread.h>
#include<sys/time.h>    



//a node in the huffman tree
struct node 
{
    char identity; 
    int flag;
    int num;
    struct node* left;
    struct node* right;
};

//for passing input to function through thread
struct input
{
    FILE *newf;
    FILE *curr;
    char *fname;
};


//functions to create huffman tree from text and generate encodings from the tree
struct node* init();
void insert_hp(struct node* y, struct node  **arr, int *curr);
void remove_min_hp(struct node  **arr, int* curr);
void make_heap(struct node **arr,int* curr, int *freq);
struct node* make_hf_tree(struct node **arr, int *curr);
void generate_encodings(struct node *node, int** encoding, int str[100], int i);

//functions to compress files and store corresponding encodings tables into separate files
struct input* init_inp();
void read_file(int* freq, FILE *ptr);
void compress(int **encodings, FILE *ptr, FILE * newf);
void store_encodings( int **encodings, char *fname2, int *freq);
void *zip(void *arg);
