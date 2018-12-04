#include "ptask.h"

void time_add_ms(struct timespec *t, int ms) {
    t->tv_sec += ms / 1000;
    t->tv_nsec += (ms % 1000) * 1000000;
    if (t->tv_nsec > 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

int task_create(Task *t) {
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    param.sched_priority = (*t).priority;
    pthread_attr_setschedparam(&attr, &param);

    return pthread_create(&(*t).id, &attr, (*t).f, NULL);
}