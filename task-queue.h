//------------------------------------------------------------------------- 
// This is supporting software for CS415/515 Parallel Programming.
// Copyright (c) Portland State University.
//------------------------------------------------------------------------- 

// Task queue representation
// -------------------------
//

// Each task holds a pair of integer values and
//   a pointer to next task.
// 
typedef struct task_ task_t;
struct task_ {
  int low;        // value 1
  int high;       // value 2
  task_t *next;   // pointer to the next task
}; 

// A queue has two pointers and two integer fields.
// 
typedef struct queue_ queue_t;
struct queue_ {
  task_t *head;   // head pointer
  task_t *tail;   // tail pointer
  int limit;      // queue capacity (0 means unlimited)
  int length;     // number of tasks in queue
}; 

// Task queue routines
// (They are defined in task-queue.c)
//
extern task_t *create_task(int low, int high);
extern queue_t *init_queue(int limit);
extern int add_task(queue_t *queue, task_t *task);
extern task_t *remove_task(queue_t *queue);
