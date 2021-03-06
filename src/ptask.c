/* Functions taken from slides */

#include "ptask.h"

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
void wait_for_activation(struct timespec *t, struct timespec *dl, int p) {
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, t, NULL);
    time_add_ms(dl, p);
    time_add_ms(t, p);
}

/* Inizialization of time with given period */
void set_activation(struct timespec *t, int p) {
    clock_gettime(CLOCK_MONOTONIC, t);
    time_add_ms(t, p);
}

/* Comparison between time, return 1 when the first argument is greater than the
 * second, -1 when the first argument is less than the second and 0 when the two
 * arguments are egual */
int time_cmp(struct timespec t1, struct timespec t2) {
    if (t1.tv_sec > t2.tv_sec)
        return 1;
    if (t1.tv_sec < t2.tv_sec)
        return -1;
    if (t1.tv_nsec > t2.tv_nsec)
        return 1;
    if (t1.tv_nsec < t2.tv_nsec)
        return -1;
    return 0;
}

/* Check for deadline miss */
int deadline_miss(struct timespec dl) {
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    if (time_cmp(now, dl) > 0) {
        return 1;
    }

    return 0;
}

/* Check for deadline miss and in case increase the value in the task table */
void check_deadline(struct timespec *dl, size_t index) {
    struct timespec now;
    size_t ex_time;

    pthread_mutex_lock(&mutex_tt);

    // Update the deadline miss
    task_table[index].dmiss += deadline_miss(*dl);

    // Compute the WCET
    clock_gettime(CLOCK_MONOTONIC, &now);
    ex_time = task_table[index].period + (now.tv_sec - dl->tv_sec) * 1000 +
           ((now.tv_nsec - dl->tv_nsec)) / 1000000;
    task_table[index].WCET =
        (ex_time > task_table[index].WCET) ? ex_time : task_table[index].WCET;

    pthread_mutex_unlock(&mutex_tt);
}

/* Create new thread with parameters of task */
int task_create(Task *t) {
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    param.sched_priority = (*t).priority;
    pthread_attr_setschedparam(&attr, &param);

    return pthread_create(&(*t).id, &attr, (*t).f, NULL);
}