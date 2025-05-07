/********************************************************************
* Linked List
*
*
* Warcom Soft. 07/05/2025
********************************************************************/
#ifndef _H_LIST_
#define _H_LIST_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    void *data;
    struct node *next;
};

//Function to insert data at head. This functions requires the size of the data type as extra argument
struct node* insertAtHead(struct node* head, void *data, size_t data_size);

#endif