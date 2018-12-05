#ifndef SENSOR_H
#define SENSOR_H

#include "interface.h"
#include "ptask.h"
#include <termios.h>
//#include <stdint.h>

#define RANGE (256)
#define BOTTOM_LIMIT (0 + RANGE)
#define UPPER_LIMIT ((65536) - RANGE)

void *simulate_sensor_task();
void *read_from_sensor_task();

#endif // SENSOR_H