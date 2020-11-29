#ifndef _PR_Q_
#define _PR_Q_

#include "ipc.h"

typedef struct data {
    timestamp_t time;
    local_id proc_id;
} Data;
 
typedef struct node { 
    Data data;
 
    struct node* next;
 
} Node;
 
Node* newNode(timestamp_t time, local_id proc_id);
Data peek(Node** head);
void pop(Node** head);
void push(Node** head, timestamp_t time, local_id proc_id);
int isEmpty(Node** head);

#endif
