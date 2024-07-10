/*
Implementation of a self relative binary tree 
Features:
    Binary tree
    Relative Pointers :
        Customizable ptr sizes
        Self Relative thus take less space
    Uses a Fixed NodePool 
    
*/

#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<stdlib.h>
#include<errno.h>
#include <string.h>
//The max capacity of the buffer
#define MAX_CAP 1024
// Using these you can control the size of the tree
typedef int8_t NodeRelPtrType;
typedef int16_t NodeData;

/*
Assume NodeRelPtrType is int8_t 
this means it gives a range of -128 to 127
The MIN value is -128
-128 is used as the new NULL and all other values as valid values

Assume Abs ptr = 0 and loc is the loc of the rel ptr
    => ABS2RELPTR = (0 - loc)^-128
    => -loc ^ -128  (Encoded NULL )

Reverse 
    REL2ABSPTR = loc +( -loc ^ -128) ^ -128
    =>  -loc + loc
    =>  0       (Decoded NULL)
*/

#define UNUSED(x) (void)(x)
#define ROOT_INDEX 0
#define MIN_OF(type) \
    (((type)(1LLU<<((sizeof(type)<<3)-1)) < (type)1) ? (long long int)((~0LLU)-((1LLU<<((sizeof(type)<<3)-1))-1LLU)) : 0LL)
#define MIN_NODE_REL_PTR_VAL  MIN_OF(NodeRelPtrType)
#define ABS2RELPTR(relptr , val ) relptr = (NodeRelPtrType)((uint8_t*)(val) - (uint8_t*)(&relptr)) ^ MIN_NODE_REL_PTR_VAL
#define REL2ABSPTR(Type, absptr) (Type *)((uint8_t*)(&absptr) + (absptr ^ MIN_NODE_REL_PTR_VAL))
typedef struct Node Node;
typedef struct Node {
    NodeData data;
    NodeRelPtrType left;            //This is a relative pointer that stores the offset of the next node relative to it's own address
    NodeRelPtrType right;
} Node;

// A nodepool 
typedef struct NodePool{
    Node buffer[MAX_CAP];
    size_t top;
}NodePool;
NodePool global_pool = {0};


Node * alloc_node(NodePool * np ){
    assert(np->top < MAX_CAP);
    return &(np->buffer[np->top++]);
}

Node * alloc_node_with_data(NodePool * np,NodeData data ){
    Node * node = alloc_node(np);
    memset(node , 0 , sizeof(Node));
    node->data = data;
    return node;
}
Node * insert_binary(Node * root , NodeData data){
    UNUSED(root);
    UNUSED(data);
    return NULL;
}

void print_tree(Node * root,size_t level){

    for (size_t i =0;i<level;i++) 
        fputc('\t',stdout);
    // printf("%lld\tleft->%p(0x%x + %p)\tright->%p(0x%x + %p)\n",root->data,REL2ABSPTR(Node,root->left),root->left,(uint8_t*)&root->left,REL2ABSPTR(Node,root->right),root->right,(uint8_t*)&root->left);
    printf("%lld\n",root->data);
    fflush(stdout);
    if (root->left) print_tree(REL2ABSPTR(Node, root->left),level + 1);
    if (root->right) print_tree(REL2ABSPTR(Node, root->right),level + 1);
}

Node *  generate_random_tree_of_level(NodePool* np , size_t level){
    
    
    NodeData data = rand()%100;
    Node * root  = alloc_node_with_data(np , data);
    for (size_t i=0 ; i< level; i++)
        fputc('\t',stdout);
    printf("GEN:%d\n",data);
    if (level>1)ABS2RELPTR(root->left,generate_random_tree_of_level(np , level - 1) );
    if (level>1)ABS2RELPTR(root->right,generate_random_tree_of_level(np , level - 1));
    return root;
}

void save_tree_to_file(NodePool * np, const char * filename){
    FILE * f = fopen(filename,"wb");
    if (f==NULL){
        fprintf(stderr , "ERROR: Cannot open file %s: %s\n",filename , strerror(errno));
        exit(1);
    }
    size_t written = fwrite(np->buffer , sizeof(Node),np->top,f);
    assert(written == np->top);
}

void load_tree_from_file(NodePool * np, const char * filename){
    FILE * f = fopen(filename,"rb");
    if (f==NULL){
        fprintf(stderr , "ERROR: Cannot open file %s: %s\n",filename , strerror(errno));
        exit(1);
    }
    fseek(f,0,SEEK_END);
    size_t num_of_bytes = ftell(f);
    assert(num_of_bytes % sizeof(Node) == 0);
    size_t num_of_nodes = num_of_bytes/ sizeof(Node);
    assert(num_of_nodes < MAX_CAP);
    np->top = num_of_nodes;
    fseek(f,0,SEEK_SET);
    fread(np->buffer, sizeof(Node),np->top,f);


}

void insert_into_tree(NodePool * np , NodeData data){
    Node * root = &(global_pool.buffer[ROOT_INDEX]);
    Node * new_node = alloc_node_with_data(np,data);
    if (root == new_node){
        return;
    }
    Node * cur_node = root;
    while (1){ // traverse down untill a null node is found
        if (data < cur_node->data){
            if (cur_node->left)
                cur_node = REL2ABSPTR(Node,cur_node->left);
            else{
                ABS2RELPTR(cur_node->left,new_node);
                return;
            }
        
        }else{
            if (cur_node->right)
                cur_node = REL2ABSPTR(Node,cur_node->right);
            else{
                ABS2RELPTR(cur_node->right,new_node);
                return;
            }
                
        }
    }



    
}

int run_load(){
    const char * OUTFILE_PATH = "gen.bin";
    printf("Loading from file %s\n",OUTFILE_PATH);
    load_tree_from_file(&global_pool , OUTFILE_PATH);
    Node * root = &(global_pool.buffer[ROOT_INDEX]);
    print_tree(root,0);
    UNUSED(root);
    return 0;
}

int run_save(){
    srand(0);
    const char * OUTFILE_PATH = "gen.bin";
    // Node * root = generate_random_tree_of_level(&global_pool , 6);
        
    Node * root = &(global_pool.buffer[ROOT_INDEX]);
    insert_into_tree(&global_pool,10);
    for (int i=0;i<10;i++){
        insert_into_tree(&global_pool,i);
    }

    
    print_tree(root,0);
    UNUSED(root);
    printf("Saving to file %s\n",OUTFILE_PATH);

    save_tree_to_file(&global_pool , OUTFILE_PATH);
    // printf("size of Node * is %zu ",sizeof(Node));
    return 0;

}

int main(){

    #ifdef TEST_SAVE
    run_save();
    #elif defined(TEST_LOAD)
    run_load();
    #endif
    return 0;
}