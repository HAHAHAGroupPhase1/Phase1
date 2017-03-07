
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <os.h>

struct node
{
    OS_TICK  key;
    OS_TCB*  p_tcb;
    OS_TICK  nextDispatch;    

    int height;

    struct node* left;
    struct node* right;
};

typedef struct node node;

node* new_node( OS_TICK  key, OS_TCB*  p_tcb, OS_TICK  nextDispatch)
{
    node* p = malloc(sizeof(*p));

    p -> key    = key;
    p -> nextDispatch = nextDispatch;
    p -> p_tcb = p_tcb;
    p -> height = 1;
    p -> left   = NULL;
    p -> right  = NULL;

    return p;
}

int max(OS_TICK a, OS_TICK b)
{
    return a > b ? a : b;
}

int height(node* p)
{
    return p ? p -> height : 0;
}

void recalc(node* p)
{
    p -> height = 1 + max(height(p -> left), height(p -> right));
}

node* rotate_right(node* p)
{
    node* q = p -> left;

    p -> left = q -> right;
    q -> right = p;

    recalc(p);
    recalc(q);

    return q;
}

node* rotate_left(node* p)
{
    node* q = p -> right;
    p -> right = q -> left;
    q -> left = p;

    recalc(p);
    recalc(q);

    return q;
}

node* balance(node* p)
{
    recalc(p);

    if ( height(p -> left) - height(p -> right) == 2 )
    {
        if ( height(p -> left -> right) > height(p -> left -> left) )
            p -> left = rotate_left(p -> left);
        return rotate_right(p);
    }
    else if ( height(p -> right) - height(p -> left) == 2 )
    {
        if ( height(p -> right -> left) > height(p -> right -> right) )
            p -> right = rotate_right(p -> right);
        return rotate_left(p);
    }

    return p;
}

node* search(node* p, OS_TICK key)
{
    if ( !p )
        return NULL;

    if ( key < p -> key )
        return search(p -> left, key);
    else if ( key > p -> key )
        return search(p -> right, key);
    else
        return p;        
}

node* insert(node* p, OS_TICK key, OS_TCB*  p_tcb, OS_TICK  nextDispatch)
{
    if ( !p )
        return new_node(key, p_tcb, nextDispatch);

    if ( key < p -> key )
        p -> left = insert(p -> left, key, p_tcb, nextDispatch);
    else if ( key > p -> key )
        p -> right = insert(p -> right, key, p_tcb, nextDispatch);
    else 
        p -> nextDispatch = nextDispatch;
        p -> p_tcb = p_tcb;
        
    return balance(p);
}

node* find_min(node* p)
{
    if ( p -> left != NULL )
        return find_min(p -> left);
    else
        return p;
}

node* remove_min(node* p)
{
    if ( p -> left == NULL )
        return p -> right;

    p -> left = remove_min(p -> left);
    return balance(p);
}

node* remove_item(node* p, OS_TICK key)
{
    if ( !p )
        return NULL;

    if ( key < p -> key )
        p -> left = remove_item(p -> left, key);
    else if ( key > p -> key )
        p -> right = remove_item(p -> right, key);
    else
    {
        node* l = p -> left;
        node* r = p -> right;
        free(p);

        if ( r == NULL )
            return l;
       
        node* m = find_min(r);
        m -> right = remove_min(r);        
        m -> left = l;
        return balance(m);
    }

    return balance(p);
}

void free_tree(node* p)
{
    if ( !p )
        return;

    free_tree(p -> left);
    free_tree(p -> right);
    free(p);
}

