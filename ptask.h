#ifndef PTASK_H
#define PTASK_H

#include <pthread.h>
#include <sched.h>
#include <time.h>

typedef struct {
    pthread_t id;
    void *f;
    int priority;
} Task;

int task_create(Task *);
void time_add_ms(struct timespec *, int);

#endif // PTASK_H