/********************************************************************
* Linked List
*
* 07/05/2025
* Warcom Soft. - warrior.rockk@gmail.com 
********************************************************************/

#include "list.h"

struct node* insertAtHead(struct node* head, void *data, size_t data_size) {
  
    // Allocate memory for node
    struct node* newNode = (struct node*)malloc(sizeof(struct node));

    newNode->data = malloc(data_size);
    newNode->next = head;

    // Copy contents of data to newly allocated memory.
    memcpy(newNode->data, data, data_size);

	return newNode;
}