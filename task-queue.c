//------------------------------------------------------------------------- 
// This is supporting software for CS415/515 Parallel Programming.
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// Task queue routines
// -------------------
//

#include <stdlib.h>
#include "task-queue.h"   // Task queue representation definition

// Create a new task
//
task_t *create_task(int low, int high) {
  task_t *task = (task_t *) malloc(sizeof(task_t));
  task->low = low;
  task->high = high;
  task->next = NULL;
  return task;
}

// Create the task queue
//
// (Note: limit <= 0 means there is no limit)
//
queue_t *init_queue(int limit) {
  queue_t *queue = (queue_t *) malloc(sizeof(queue_t));
  queue->head = queue->tail = NULL;
  queue->limit = limit;
  queue->length = 0;
  return queue;
}

// Add a task to the tail of the queue
// - If successful, return the queue's new length;
//   otherwise, return 0.
//
int add_task(queue_t *queue, task_t *task) {
  if (queue->limit <= 0 || queue->length < queue->limit) {
    if (!queue->tail) {
      queue->head = queue->tail = task;
    } else {
      queue->tail->next = task;
      queue->tail = task;
    }
    return ++queue->length;
  }
  return 0;
}

// Remove a task from the head of the queue
// - If no task in the queue, return null.
//
task_t *remove_task(queue_t *queue) {
  task_t *task = NULL;
  if (queue->length > 0) {
    task = queue->head;
    if (queue->head == queue->tail)
      queue->head = queue->tail = NULL;
    else
      queue->head = queue->head->next;
    queue->length--;
  }
  return task;
}

