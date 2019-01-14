#ifndef PTASK_H
#define PTASK_H

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <time.h>

#define SI_I (0)
#define RS_I (1)
#define G_I (2)
#define NN_I (3)
#define K_I (4)

typedef struct {
    pthread_t id;
    void *f;
    int priority;
    int period;
    size_t dmiss;
} Task;

extern Task task_table[];

void time_add_ms(struct timespec *, int);
void time_copy(struct timespec *, struct timespec);
void wait_for_activation(struct timespec *, struct timespec *, int);
void set_activation(struct timespec *, int);
void check_deadline(struct timespec *, size_t);
int task_create(Task *);

#endif // PTASK_H