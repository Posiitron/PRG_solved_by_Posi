#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

// Define the structure of a node in the linked list
typedef struct Node {
    int value;
    struct Node* next;
} Node;

// Global variables for the linked list
static Node* head = NULL; // Pointer to the head of the list
static Node* tail = NULL; // Pointer to the tail of the list
static int list_size = 0; // Number of elements in the list

// Function to push an element at the end of the queue
_Bool push(int entry) {
    if (entry < 0) return false; // Ensure non-negative entries

    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) return false; // Memory allocation failed

    new_node->value = entry;
    new_node->next = NULL;

    if (tail) {
        tail->next = new_node; // Append to the end of the list
    } else {
        head = new_node; // If the list was empty, set head to the new node
    }
    tail = new_node; // Update the tail to the new node
    list_size++; // Increase the size of the list
    return true;
}

// Function to pop an element from the front of the queue
int pop(void) {
    if (!head) return -1; // Queue is empty

    Node* temp = head;
    int value = head->value; // Get the value of the head node
    head = head->next; // Move head to the next node

    if (!head) {
        tail = NULL; // If the list is now empty, set tail to NULL
    }

    free(temp); // Free the memory of the old head node
    list_size--; // Decrease the size of the list
    return value;
}

// Function to insert an element in sorted order
_Bool insert(int entry) {
    if (entry < 0) return false; // Ensure non-negative entries

    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) return false; // Memory allocation failed
    new_node->value = entry;
    new_node->next = NULL;

    // If the list is empty or the new entry should be the new head
    if (!head || entry > head->value) {
        new_node->next = head;
        head = new_node;
        if (!tail) {
            tail = new_node; // If the list was empty, set tail to the new node
        }
        list_size++; // Increase the size of the list
        return true;
    }

    Node* current = head;
    // Traverse the list to find the correct position
    while (current->next && entry < current->next->value) {
        current = current->next;
    }

    // Insert the new node at the found position
    new_node->next = current->next;
    current->next = new_node;
    if (!new_node->next) {
        tail = new_node; // If the new node is the new tail, update tail
    }
    list_size++; // Increase the size of the list
    return true;
}

// Function to erase all elements with a specific value
_Bool erase(int entry) {
    if (!head) return false; // List is empty

    _Bool found = false;
    Node** ptr = &head; // Pointer to the pointer to the current node

    // Traverse the list to find and remove nodes with the specified value
    while (*ptr) {
        if ((*ptr)->value == entry) {
            Node* to_delete = *ptr;
            *ptr = (*ptr)->next; // Remove the node from the list
            free(to_delete); // Free the memory of the removed node
            list_size--; // Decrease the size of the list
            found = true;
        } else {
            ptr = &((*ptr)->next); // Move to the next node
        }
    }

    if (!head) {
        tail = NULL; // If the list is now empty, set tail to NULL
    }

    return found;
}

// Function to get the value at a specific index
int getEntry(int idx) {
    if (idx < 0 || idx >= list_size) return -1; // Index out of bounds

    Node* current = head;
    // Traverse the list to the specified index
    for (int i = 0; i < idx; i++) {
        current = current->next;
    }

    return current->value; // Return the value at the index
}

// Function to get the current size of the queue
int size(void) {
    return list_size; // Return the number of elements in the list
}

// Function to clear the entire queue
void clear(void) {
    Node* current = head;
    while (current) {
        Node* to_delete = current;
        current = current->next;
        free(to_delete); // Free the memory of each node
    }
    head = tail = NULL; // Reset head and tail to NULL
    list_size = 0; // Reset the size of the list
}
