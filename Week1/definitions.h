#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/stat.h> // for stat
#include <dirent.h> 
#include<stdio.h>



//structs for huffman tree
struct node 
{
    char identity; 
    int flag;
    int num;
    struct node* left;
    struct node* right;
};

//initialization 
struct node* init();

//function related to huffman coding
void insert_hp(struct node* y, struct node  **arr, int *curr);
void remove_min_hp(struct node  **arr, int* curr);
void make_heap(struct node **arr,int* curr, int *freq);
struct node* make_hf_tree(struct node **arr, int *curr);
void generate_encodings(struct node *node, int** encoding, int str[100], int i);



//functions related to runlength coding
void encode(char* filename,char* dirname,char* file_1);
void decode(char* filename,char* dirname,char* file_1);


//functions related to zip
void read_file(int* freq, FILE *ptr);
void compress(int **encodings, FILE *ptr, FILE * newf);
void store_encodings( int **encodings, char *fname2, int *freq);
void zip(FILE *ptr, FILE *newf, char* fname);

//function related to unzip
void extract_encodings_from_storage(FILE *ef,char **encds);
void convert_back_to_binary(FILE *tf, char *currfname);
void decrypt(FILE *tf, char **encds, char *currfname);
void unzip(FILE *ef, FILE *curr, char* currfname);
 

//function to check if file is directory
int dir(char* name);