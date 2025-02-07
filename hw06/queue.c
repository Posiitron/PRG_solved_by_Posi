#include "queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Function prototypes for internal use
static bool resize_queue(queue_t *queue, int new_capacity);

// Function to create a new queue with a given size
queue_t *create_queue(int capacity) {
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    if (!queue)
        return NULL;

    // Allocate one extra space to distinguish between full and empty states
    queue->elements = (void **)malloc(sizeof(void *) * (capacity + 1));
    if (!queue->elements) {
        free(queue);
        return NULL;
    }

    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0; // Initialize rear to 0 instead of -1
    queue->size = 0;
    queue->initial_size = capacity; // Initialize initial_size

    return queue;
}

// Function to delete the queue and all allocated memory
void delete_queue(queue_t *queue) {
    if (queue) {
        free(queue->elements);
        free(queue);
    }
}

// Function to check if the queue is full
bool queue_full(const queue_t *queue) {
    // Check if the next position is the front, which means the queue is full
    return ((queue->rear + 1) % (queue->capacity + 1)) == queue->front;
}

// Function to check if the queue is empty
bool queue_empty(const queue_t *queue) {
    // If the front and rear are equal, the queue is empty
    return queue->front == queue->rear;
}

// Function to insert an element into the queue
bool push_to_queue(queue_t *queue, void *data) {
    if (queue_full(queue)) {
        // Resize to twice the current capacity
        if (!resize_queue(queue, queue->capacity * 2)) {
            return false; // Return false if memory allocation fails
        }
    }

    queue->elements[queue->rear] = data;
    queue->rear = (queue->rear + 1) % (queue->capacity + 1);
    queue->size++;

    return true;
}

// Function to get the first element from the queue and remove it
void *pop_from_queue(queue_t *queue) {
    if (queue_empty(queue))
        return NULL;

    void *data = queue->elements[queue->front];
    queue->front = (queue->front + 1) % (queue->capacity + 1);
    queue->size--;

    // Shrink to one third if the size falls below one fourth of the capacity
    if (queue->size < queue->capacity / 4) {
        // Resize to one third of the current capacity, but not less than the initial size
        int new_capacity = queue->capacity / 3 > queue->initial_size ? queue->capacity / 3 : queue->initial_size;
        resize_queue(queue, new_capacity);
    }

    return data;
}

// Function to get the idx-th element from the queue
void *get_from_queue(queue_t *queue, int idx) {
    if (idx < 0 || idx >= queue->size)
        return NULL;

    int index = (queue->front + idx) % (queue->capacity + 1);
    return queue->elements[index];
}

// Function to get the number of stored elements
int get_queue_size(queue_t *queue) {
    return queue->size;
}

// Internal function to resize the queue
static bool resize_queue(queue_t *queue, int new_capacity) {
    // Allocate a new array for elements with one extra space
    void **new_elements = (void **)malloc(sizeof(void *) * (new_capacity + 1));
    if (!new_elements)
        return false;

    // Initialize new_elements to NULL to avoid uninitialized reads
    for (int i = 0; i < new_capacity + 1; i++) {
        new_elements[i] = NULL;
    }

    // Copy the elements from the old to the new array
    for (int i = 0; i < queue->size; i++) {
        int old_index = (queue->front + i) % (queue->capacity + 1);
        new_elements[i] = queue->elements[old_index];
    }

    // Free the old elements array
    free(queue->elements);

    // Update the queue structure
    queue->elements = new_elements;
    queue->capacity = new_capacity;
    queue->front = 0;
    queue->rear = queue->size; // Rear is adjusted to the new last element

    return true;
}
