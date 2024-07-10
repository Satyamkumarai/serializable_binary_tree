/*
An attempt to create a memory mapped file.
This file loads a memory mapped file to load a tree directly into memory.
*/
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<string.h>
#include<errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#ifndef NODE_POOL_CAP
#define NODE_POOL_CAP 20
#endif

typedef int32_t Node_data_type ;
typedef uint32_t Node_ptr_type ;
#define ROOT_INDEX 0
typedef struct Node {
    Node_data_type data;
    Node_ptr_type left;
    Node_ptr_type right;
}Node;

typedef struct NodePool { 
    Node buffer[NODE_POOL_CAP];
    size_t top;
}NodePool;

NodePool global_pool;

Node_ptr_type  alloc_node(NodePool * pool){
    assert(pool->top < NODE_POOL_CAP );
    Node * node = &pool->buffer[pool->top];
    memset(node, 0, sizeof(Node));
    // node->data = data;
    return pool->top++;
    
}

Node * insert_tree(NodePool * pool , Node_data_type data ){
    Node_ptr_type offset = alloc_node(&global_pool);
    //allocate the node and insert data to it 
    Node * node = &(pool->buffer[offset]);
    node->data = data;

    if (offset == ROOT_INDEX){
        return &(pool->buffer[ROOT_INDEX]);
    }
    Node * c_node = &(pool->buffer[ROOT_INDEX]);
    while (1){
        //otherwise if the data is smaller insert left 
        if (data < c_node->data ){   
            //if left already has data  
            if (c_node->left){
                c_node = &(pool->buffer[c_node->left]);
            }else{ //we found an empty left node 
                c_node->left = offset;
                c_node = &(pool->buffer[c_node->left]);

                return c_node;
            }

        }else{
            // try to insert it at right 
            if (c_node->right){
                c_node = &(pool->buffer[c_node->right]);

            }else{ //we found an empty right node 
                c_node->right = offset;
                c_node = &(pool->buffer[c_node->right]);
                return c_node;
            }
        }
        
    }
}


void print_tree(FILE * stream,NodePool * pool ,size_t level,Node_data_type offset){

    Node * cnode = &(pool->buffer[offset]);  
    for (size_t i=0;i<level;i++)
        putc('\t',stream);
    fprintf(stream,"%d\n",cnode->data);


    if (cnode->left)
        print_tree(stream,pool , level+1,cnode->left);
    else{
        for (size_t i=0;i<level+1;i++)
            putc('\t',stream);
        fprintf(stream,"X\n");

    }
    if (cnode->right)
        print_tree(stream,pool,level+1, cnode->right);
    else{
        for (size_t i=0;i<level+1;i++)
            putc('\t',stream);
        fprintf(stream,"X\n");

    }

}

void save_pool_to_file(NodePool * pool,const char * filename ){
    FILE * outfile = fopen(filename, "wb");
    if (outfile == NULL){
        fprintf(stderr ,"ERROR: cannot write to file%s: %s\n",filename , strerror(errno) );
        return;
    }
    size_t n = fwrite(pool->buffer,sizeof(Node),pool->top,outfile);
    assert(n==pool->top);
    
}
void load_pool_from_file( NodePool * pool, const char * filename){
    FILE * outfile = fopen(filename, "rb");
    if (outfile == NULL){
        fprintf(stderr ,"ERROR: cannot write to file%s: %s\n",filename , strerror(errno) );
        return;
    }
    fseek(outfile,0 , SEEK_END);
    size_t buffer_size = ftell(outfile);
    assert (buffer_size % sizeof(Node) == 0 );
    size_t num_of_nodes = buffer_size / sizeof(Node);
    assert(num_of_nodes < NODE_POOL_CAP);
    fseek(outfile , 0 ,SEEK_SET);
    pool->top = num_of_nodes;
    fread(pool->buffer,sizeof(Node),pool->top ,outfile );

    }


int main (){
    // filename 
    const char * filename = "out.bin";
    const char * print_file_name = "tree.txt";
    FILE * outfile = fopen(print_file_name,"w");
    int  fd = open(filename, O_RDONLY);
    struct stat file_info;
    //get the file_info
    assert (fstat (fd, &file_info) != -1);
    //create a private m- mapped memory

    NodePool * mmap_addr =(NodePool *) mmap (NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert (mmap_addr != MAP_FAILED);
    printf("Printing Tree to file %s",print_file_name);
    print_tree(outfile,mmap_addr,0,ROOT_INDEX);


    // #ifdef TEST_SAVE
    
    // Node * root = insert_tree(&global_pool , 100);
    // for (int i=1 ;i<10;i++)
    //     if (insert_tree (&global_pool , i)==NULL)
    //         printf("Error inserting%d \n",i);
    // print_tree(&global_pool , 0 , ROOT_INDEX);
    // printf("Saving to file : %s\n",filename);
    // save_pool_to_file(&global_pool,filename);

    // #elif defined(TEST_LOAD)

    // Node * root = &global_pool.buffer[ROOT_INDEX];
    // printf("Loading from file : %s\n",filename);
    // load_pool_from_file(&global_pool,filename);
    // print_tree(&global_pool , 0 , ROOT_INDEX);
    // #endif

    munmap (mmap_addr, file_info.st_size);
    close (fd);
    fclose(outfile);

    return 0 ;
}