#include "sensor.h"

/* Initialization of the queue for values that will be sampled */
void init_queue() {
    int i;
    const int offset =
        (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) /
              GRAPH_ELEMENT); // distance between values along abscissa

    pthread_mutex_lock(&mutex_data);
    r_data.top = 0;

    // inizialization of abscissa values
    for (i = 0; i < GRAPH_ELEMENT; ++i)
        r_data.x_point[i] = GRAPH_X1 + INTERNAL_MARGIN + i * offset;

    pthread_mutex_unlock(&mutex_data);
}

void *read_from_sensor_task(void *period) {
    struct timespec t;
    int port = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    uint8_t var[4];

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, *(int *)period);

    struct termios options;

    // set port attribute
    tcgetattr(port, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    cfmakeraw(&options);
    tcsetattr(port, TCSANOW, &options);

    // initialize x value of the queue
    init_queue();

    while (1) {
        pthread_mutex_lock(&mutex_data);
        read(port, (void *)&var[0], 4);
        r_data.co2[r_data.top] = var[1] << 8 | var[0];
        r_data.tvoc[r_data.top] = var[3] << 8 | var[2];
        r_data.top = ++r_data.top % GRAPH_ELEMENT;
        pthread_mutex_unlock(&mutex_data);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, *(int *)period);
    }
}

void *simulate_sensor_task() {
    struct timespec t;
    int period = 150;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    const uint16_t v_rif_1 = 4000;

    const uint16_t v_rif_2 = 10000;

    init_queue();

    while (1) {
        pthread_mutex_lock(&mutex_data);
        r_data.co2[r_data.top] = v_rif_1 + (rand() % (2 * RANGE));
        r_data.tvoc[r_data.top] = v_rif_2 + (rand() % (2 * RANGE));
        r_data.top = ++r_data.top % GRAPH_ELEMENT;
        pthread_mutex_unlock(&mutex_data);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}
