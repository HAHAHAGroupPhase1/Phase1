
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "FiboHeap.h"

/* -----------------NODES-----------------*/

node* node_init(OS_TICK key,  OS_TCB* value, OS_TICK nextDispatch){
    node* newNode = malloc(sizeof(node));
    newNode->left = newNode->right = newNode;
    newNode->key = key;
    newNode->value = value;
    newNode->parent = NULL;
    newNode->kid = NULL;
    newNode->hasLostKid = 0;
    newNode->degree = 0;
    newNode->nextDispatch = nextDispatch;
    return newNode;
}

void node_free(node* toFree){
    toFree->degree = -1;
    free(toFree);
}

void node_kill(node* toKill){
    node* kid = toKill->kid;
    if (kid){
        kid->left->right = NULL;
        while(kid->right != NULL){
            kid = kid->right;
            node_kill(kid->left);
        }
        node_kill(kid);
    }
    node_free(toKill);
}

void node_add(node* old, node* newRight){
    node* oldRight = old->right;
    assert(old != newRight);
    assert(oldRight != newRight);
    old->right = newRight;
    oldRight->left = newRight;
    newRight->left = old;
    newRight->right = oldRight;
}

/* -----------------HEAP-----------------*/


heap* heap_init(){ //initialize heap
    return NULL;
}

int is_empty(heap* H){
    return H == NULL; // check if heap is empty
}




elem* heap_add(heap** H, node* newNode){ //add new node 
    assert(H);
    assert(newNode);
    node* oldNode = *H;
    newNode->parent = NULL;
    newNode->hasLostKid = 0;
    if (oldNode){ //nonempty heap
        node_add(oldNode, newNode);
        if (oldNode->key > newNode->key){ //new smallest
            *H = newNode;
        }
    }else{ //previously empty heap
        newNode->left = newNode;
        newNode->right = newNode;
        *H = newNode;
    }
    return newNode;
}
elem* heap_insert(heap** H, OS_TICK key, OS_TCB* value, OS_TICK nextDispatch){ //insert node using heap add
    node* newNode = node_init(key, value, nextDispatch);
    return heap_add(H, newNode);
}

data  heap_min(heap* H){ // return min node in heap
    assert(H);
    data d;
    node* head = H;
    d.key = head->key;
    d.value = head->value;
    return d;
}

data  elem_data(elem* x){
    assert(x);
    data d;
    d.key = x->key;
    d.value = x->value;
    d.nextDispatch = x->nextDispatch;
    
    return d;
}
heap* heap_union(heap* H1, heap* H2){
    if(!H1) return H2;
    if(!H2) return H1;
    if (heap_min(H2).key < heap_min(H1).key){
        return heap_union(H2, H1);
    }

    node* H1first = H1;
    node* H1last = H1first->left;
    node* H2first = H2;
    node* H2last = H2first->left;

    H1last->right = H2first;
    H2first->left = H1last;
    H2last->right = H1first;
    H1first->left = H2last;

    return H1first;
}

void  heap_remove_from(heap** H, node* x){ //subfunction to eliminate
    assert(!x->parent);
    if (x->right == x){
        *H = NULL;
    }else{
        x->left->right = x->right;
        x->right->left = x->left;
        *H = x->right;
    }
    x->left = x;
    x->right = x;
    x->parent = NULL;
}

node* heap_link(heap** H, node* x, node* y){
    assert(x);
    assert(y);
    assert(x->degree == y->degree);
    if (x->key > y->key){
        return heap_link(H, y, x);
    }
    heap_remove_from(H, y);
    if (x->kid){
        node* z = x->kid;
        y->right = z;
        y->left = z->left;
        z->left->right = y;
        z->left = y;
    }
    y->parent = x;
    x->kid = y;
    x->degree++;
    y->hasLostKid = 0;
    return x;
}

void heap_match_degrees(heap** H, node** A, node* x){
    int d = x->degree;
    while(A[d]){
        if (d > 99){
            exit(1);
        }
        node* y = A[d];
        if (y != x){
            x = heap_link(H, x, y);
            A[d] = NULL;
            d++;
        }else{
            break;
        }
    }
    A[d] = x;
}

void  heap_consolidate(heap** H){
    node* x = *H;
    if (!x) return;
    node** A = calloc(100, sizeof(node));
    memset(A, '\0', 100);
    assert(x->degree >= 0);
    node* last = x->left;
    while(x != last){
        node* next = x->right;
        heap_match_degrees(H, A, x);
        x = next;
    }
    heap_match_degrees(H, A, last);
    *H = heap_init();
    for (int i=0; i<100; i++){
        if (A[i]){
            heap_add(H, A[i]);
        }
    }
    free(A);
}

data  heap_extract_min(heap** H){ //extract min
    assert(H && *H);
    node* z = *H;
    data d = elem_data(z);
    node* first = z->kid;
    heap_remove_from(H, z);
    node_free(z);
    if (first){
        node* current = first->right;
        while (current != first){
            current->parent = NULL;
            current = current->right;
        }
        first->parent = NULL;
        *H = heap_union(*H, first);
    }
    heap_consolidate(H);
    return d;
}



void  heap_decrease_key(heap** H, elem* x, OS_TICK newKey){ //decrease key
    assert(H && *H);
    assert(x && x->key >= newKey);
    x->key = newKey;
    if(x->parent && x->parent->key > newKey){
        if (x->left == x){
            assert(x->parent->degree == 2);
            x->parent->kid = NULL;
        }else{
            assert(x->parent->degree > 2);
            x->left->right = x->right;
            x->right->left = x->left;
            x->parent->kid = x->left;
        }
        x->parent->degree--;
        heap_add(H, x);
        if (! x->parent->hasLostKid){
            x->parent->hasLostKid = 1;
        }else{
            heap_decrease_key(H, x->parent, x->parent->key);
        }

    }else{
        if (newKey < (*H)->key){
            assert(!x->parent);
            *H = x;
        }
    }
}

void  heap_delete(heap** H, elem* x){
    heap_decrease_key(H, x, INT_MIN);
    heap_extract_min(H);
}



void heap_free(heap** H){
    node* header = *H;
    node* first = header;
    if (header){
        while(header != first){
            node* next = header->right;
            node_kill(header);
            header = next;
        }
    }
    *H = NULL;
}


