#ifndef PTASK_H
#define PTASK_H

#include <pthread.h>
#include <sched.h>
#include <time.h>

typedef struct {
    pthread_t id;
    void *f;
    int priority;
    int period;
} Task;

int task_create(Task *);
void time_add_ms(struct timespec *, int);
void time_copy(struct timespec *, struct timespec);
void wait_for_activation(struct timespec *, int);
void set_activation(struct timespec *, int);

#endif // PTASK_H