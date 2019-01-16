#ifndef SENSOR_H
#define SENSOR_H

#include "interface.h"
#include "ptask.h"
#include <termios.h>

// Interval between the values read from the sensor
#define BOTTOM_LIMIT (0)
#define UPPER_LIMIT (8192)

// Distance between values along abscissa
#define OFFSET ((int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT))

#define SERIAL_PORT ("/dev/ttyACM0") // Path to serial port

extern Task task_table[]; // Link to task table initialized into main file

void *simulate_sensor_task();
void *read_from_sensor_task();

#endif // SENSOR_H