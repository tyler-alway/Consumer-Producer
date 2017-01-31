#include "task-queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include <sched.h>		// for getting cpu id

//Shared memeory
queue_t *task_list;
pthread_mutex_t lock;
pthread_cond_t cond1; //Condition for the consumer thread
pthread_cond_t cond2; //Condition for the producer thread

int *consumer_counts;
int finished_producing;


void producer() {

  task_t * temp; // A temp task

  printf("Producer starting on core %d\n", sched_getcpu());

  for(int i = 0; i < 100; i++) {
    pthread_mutex_lock(&lock);


    temp = create_task(i, i); //Create the task

    //Check if the producer can add to the queue
    //If not go to sleep
    while (add_task(task_list, temp) == 0) { //Add the task to to queue
      pthread_cond_wait(&cond2, &lock);
    }
    //printf("add task %d\n", i);
    pthread_cond_broadcast(&cond1);
    pthread_mutex_unlock(&lock);
  }

  //accuire the lock to flag the end of producing
  pthread_mutex_lock(&lock);
  finished_producing = 1;
  pthread_mutex_unlock(&lock);

  //Wake all consumers up to trerminate
  pthread_cond_broadcast(&cond1);
  printf("Producer ending\n");
}


void consumer(long tid) {

  int done;
  int count = 0;//The number of task this consumer processed
  task_t * temp; // A temp task

  printf("Consumer [%ld] started on core %d\n", tid, sched_getcpu());

  pthread_mutex_lock(&lock);
  done = finished_producing;
  pthread_mutex_unlock(&lock);

  while(!done || task_list->length != 0){

    pthread_mutex_lock(&lock);
    //If there is no work to be done go to sleep
    if (task_list->length == 0 && !finished_producing) {
      pthread_cond_wait(&cond1, &lock);
    }
    //check that there is work to be done and consume the task
    if(task_list->length != 0) {
      temp = remove_task(task_list); //remove a task from the queue
      count++;

      //printf("thread %ld removes task %d\n", tid, temp->low);

      //signal the producer to wake up after a task has been consumed
      pthread_cond_signal(&cond2);
    }
    pthread_mutex_unlock(&lock);

    //accuire the lock to see if the producer is done
    pthread_mutex_lock(&lock);
    done = finished_producing;
    pthread_mutex_unlock(&lock);
  }
  consumer_counts[tid] = count;
  printf("Consumer [%ld] ending\n", tid);

}


int main(int argc, char **argv) {
  int numThreads;
  long i;
  int count = 0;
  finished_producing = 0; //sets finished_producing to false

  //set the capacity of the queue to 20
  task_list = init_queue(20);

  pthread_mutex_init(&lock, NULL);   // initialize mutex
  pthread_cond_init(&cond1, NULL);
  pthread_cond_init(&cond2, NULL);

  // check command-line for user input
  if (argc > 1) {
    if ((numThreads=atoi(argv[1])) < 1) {
      printf ("<numThreads> must be greater than 0\n");
      exit(0);
    }
  } else {
    numThreads = 1;
  }

  //Array of consumer threads
  pthread_t thread[numThreads];
  consumer_counts = malloc(sizeof(int)*numThreads);

  for (i = 0; i < numThreads; i++) {
    pthread_create(&thread[i], NULL, (void*)consumer, (void*)i);
  }

  //main thread becomes the producer
  producer();

  //once the main thread is done producing wait for the consumner threadds to finish and join
  for (i = 0; i < numThreads; i++) {
  	pthread_join(thread[i], NULL);
  }

  for(i = 0; i < numThreads; i++) {
    printf("C[ %ld]: %d, ", i, consumer_counts[i]);
    count += consumer_counts[i];

    if (i%4 == 3) {
      printf("\n");
    }
  }

  printf("\ntotal: %d\n", count);


  return 0;
}
