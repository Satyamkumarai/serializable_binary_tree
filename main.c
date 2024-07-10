/*
    This is an implementation of a serializable binary tree using a relative pointer and a fixed static Node Pool
*/
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>
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
    if (cnode->right)
        print_tree(stream,pool,level+1, cnode->right);
    

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
    const char * filename = "out.bin";
    const char * print_file_name = "tree.txt";
    FILE * outfile = fopen(print_file_name,"w");
    if (outfile == NULL){
        fprintf(stderr, "ERROR: Could not open %s: %s\n",print_file_name,strerror(errno));
    }
    srand(0);
    #ifdef TEST_SAVE
    
    Node * root = insert_tree(&global_pool , rand());
    if (root == NULL ){
        fprintf(stderr , "Could not allocate mem for root..\n");
        return 1 ;
    }
    printf("NODE_POOL_CAP=%d\n",NODE_POOL_CAP);
    for (int i = 1 ;i < NODE_POOL_CAP-1;i++)
        if (insert_tree (&global_pool , rand())==NULL)
            printf("Error inserting%d \n",i);
    
    // print_tree(outfile,&global_pool , 0 , ROOT_INDEX);
    printf("Saving to file : %s\n",filename);
    save_pool_to_file(&global_pool,filename);

    #elif defined(TEST_LOAD)

    Node * root = &global_pool.buffer[ROOT_INDEX];
    printf("Loading from file : %s\n",filename);
    load_pool_from_file(&global_pool,filename);
    // print_tree(outfile,&global_pool , 0 , ROOT_INDEX);
    #endif
    fclose(outfile);
    return 0 ;
}