#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "ipc.h"
 

Node* newNode(timestamp_t time, local_id proc_id)
{
    Node* temp = (Node*)malloc(sizeof(Node));

    temp->data.time=time;
    temp->data.proc_id=proc_id;
    temp->next = NULL;
 
    return temp;
}
 

Data peek(Node** head)
{
    return (*head)->data;
}
 

void pop(Node** head)
{
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}
 

void push(Node** head, timestamp_t time, local_id proc_id)
{
    if(*head == NULL){
        *head = newNode(time,proc_id);
        return;
    }
    Node* start = (*head);
 
    Node* temp = newNode(time, proc_id);

    if ((*head)->data.time > time 
        || ((*head)->data.time == time && (*head)->data.proc_id > proc_id)) {
 
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        while (start->next != NULL &&
               (start->next->data.time < time
               ||(start->next->data.time == time && start->next->data.proc_id < proc_id))) {
            start = start->next;
        }

        temp->next = start->next;
        start->next = temp;
    }
}
 
int isEmpty(Node** head)
{
    return (*head) == NULL;
}
