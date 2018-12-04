#include "sensor.h"

/* Initialization of the queue for values that will be sampled */
void init_queue() {
    int i;
    const int offset =
        (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) /
              GRAPH_ELEMENT); // distance between values along abscissa

    pthread_mutex_lock(&mutex_data);
    r_data.top = r_data.first = 0;

    // inizialization of abscissa values
    for (i = 0; i < GRAPH_ELEMENT; ++i)
        r_data.x_point[i] = GRAPH_X1 + INTERNAL_MARGIN + i * offset;

    pthread_mutex_unlock(&mutex_data);
}

void *simulate_sensor_task() {
    struct timespec t;
    int period = 150;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    const unsigned long v_rif = 6000;

    init_queue();

    while (1) {
        pthread_mutex_lock(&mutex_data);
        r_data.elem[r_data.top] = v_rif + (rand() % (2 * RANGE));
        r_data.top++;
        r_data.top %= GRAPH_ELEMENT;
        pthread_mutex_unlock(&mutex_data);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}