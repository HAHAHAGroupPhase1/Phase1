
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <os.h>

typedef struct{
    OS_TICK key;     
    OS_TCB* value; 
    OS_TICK nextDispatch;
    
} data;
/* -----------------NODES-----------------*/

typedef struct node node;
struct node{
  node* left;
  node* right;
  node* parent; // NULL iff this node is a root
  node* kid; // Any kid
  OS_TICK key;
  OS_TCB* value; // Arbitrary heap client data
  OS_TICK nextDispatch;
  int hasLostKid; //Boolean, aka "mark"
  int degree; //number of kids
};

typedef node heap;
typedef node elem;

//Don't pass any NULL pointers to these functions. 

node* node_init(OS_TICK key, OS_TCB* value, OS_TICK nextDispatch);

//node_free deallocates the node. Will leak memory and leave dead pointers.
//node_kill recursively kills its children and then frees itself.
void  node_free(node* toFree);
void  node_kill(node* toKill);

//Adds root newRight to the circularly linked list, right of old
void node_add(node* old, node* newRight);

/* -----------------HEAP FUNCTION PROTOTYPES-----------------*/

//Initialise
heap* heap_init();
int is_empty(heap* H);


//Insert: returns a pointer to the element inserted, for decrease_key and delete
elem* heap_add(heap** H, node* newNode);
elem* heap_insert(heap** H, OS_TICK key,  OS_TCB* value, OS_TICK nextDispatch ); //insert node with 
node* heap_link(heap** H, node* x, node* y); //link

//Delete Min: don't call on empty heap
data  heap_min(heap* H); //returen min node
data  heap_extract_min(heap** H); //extract min node
heap* heap_union(heap* H1, heap* H2); //unite two heaps

//Decrease Key
void  heap_decrease_key(heap** H, elem* x, OS_TICK newKey);

//Delete
void  heap_delete(heap** H, elem* x); //delete element x
data  elem_data(elem* x);//return key and value of x
void  elem_set_value(elem* x, void* newValue); //set new value to x

void  heap_free(heap** H);
void  heap_consolidate(heap** H);
void  heap_match_degrees(heap** H, node** A, node* x);
void  heap_remove_from(heap** H, node* x);



