#include "sensor.h"

/* Initialization of the queue for values that will be sampled */
void init_queue() {
    int i;

    pthread_mutex_lock(&mutex_data);
    r_data.top = 0; // inizialization of the top of the queue

    // Inizialization the abscissa values
    for (i = 0; i < GRAPH_ELEMENT; ++i)
        r_data.x_point[i] = GRAPH_X1 + INTERNAL_MARGIN + i * OFFSET;

    pthread_mutex_unlock(&mutex_data);
}

/* Open the serial port and inizialization its attributes */
int serial_init(struct termios *options) {
    int port; // Id of serial port

    // Open serial port
    port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);

    // Get default serial port attributes
    tcgetattr(port, options);

    // Set input and output baud rate
    cfsetispeed(options, B115200);
    cfsetospeed(options, B115200);

    // Set raw mode
    cfmakeraw(options);

    // Commit the port attributes immediately
    tcsetattr(port, TCSANOW, options);

    return port;
}

void *read_from_sensor_task() {
    struct timespec t;  // Time refering the period
    struct timespec dl; // Time refering the deadline
    struct termios options; // Options for correctly configure the serial port
    int port;               // Id to serial port
    uint8_t var[4];         // Value sampled from sensor

    // Inizialization of serial port
    port = serial_init(&options);

    // Initialize x values of the queue
    init_queue();

    set_activation(&t, task_table[RS_I].period);
    set_activation(&dl, task_table[RS_I].period);

    while (1) {
        pthread_mutex_lock(&mutex_data);

        // Read values sampled from sensor
        read(port, (void *)&var[0], 4);

        // Insert sampled values on the top of the queue
        r_data.co2[r_data.top] = var[1] << 8 | var[0];
        r_data.tvoc[r_data.top] = var[3] << 8 | var[2];

        // Update the index of the top of the queue
        r_data.top = (r_data.top + 1) % GRAPH_ELEMENT;
        pthread_mutex_unlock(&mutex_data);

        check_deadline(&dl, RS_I);
        wait_for_activation(&t, &dl, task_table[RS_I].period);
    }
}