
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    OS_TICK key;     // Indicates the minimum
    char* value; //Arbitrary client data
} data;
//The representation of the node is exposed to facilitate pointer manipulation.
//However functions on nodes are given for other operations.

typedef struct node node;
struct node{
  node* left;
  node* right;
  node* parent; // NULL iff this node is a root
  node* kid; // Any kid
  OS_TICK key;
  char* value; // Arbitrary heap client data
  int hasLostKid; //Boolean, aka "mark"
  int degree; //number of kids
};

typedef node heap;
typedef node elem;

//Don't pass any NULL pointers to these functions. 

node* node_init(OS_TICK key, char* value);

//node_free deallocates the node. Will leak memory and leave dead pointers.
//node_kill recursively kills its children and then frees itself.
void  node_free(node* toFree);
void  node_kill(node* toKill);

//Adds root newRight to the circularly linked list, right of old
void node_add(node* old, node* newRight);

