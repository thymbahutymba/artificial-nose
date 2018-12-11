#include "ptask.h"

/* Function from slides */

/* Add ms to time */
void time_add_ms(struct timespec *t, int ms) {
    t->tv_sec += ms / 1000;
    t->tv_nsec += (ms % 1000) * 1000000;
    if (t->tv_nsec > 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

/* Copy the current time of source into dest */
void time_copy(struct timespec *t_dst, struct timespec t_src) {
    t_dst->tv_sec = t_src.tv_sec;
    t_dst->tv_nsec = t_src.tv_nsec;
}

/* Suspend task until new activation */
void wait_for_activation(struct timespec *t, int p) {
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t, NULL);
    time_add_ms(t, p);
}

/* Inizialization of time with given period */
void set_activation(struct timespec *t, int p) {
    clock_gettime(CLOCK_MONOTONIC, t);
    time_add_ms(t, p);
}

/* Create new thread with parameter of task */
int task_create(Task *t) {
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    param.sched_priority = (*t).priority;
    pthread_attr_setschedparam(&attr, &param);

    return pthread_create(&(*t).id, &attr, (*t).f, (void *)&t->period);
}