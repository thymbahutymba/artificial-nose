#ifndef SENSOR_H
#define SENSOR_H

#include "interface.h"
#include "ptask.h"
#include <termios.h>
//#include <stdint.h>

#define RANGE (2048)        // 1 << 11
#define BOTTOM_LIMIT (0)
#define UPPER_LIMIT (65536) // 1 << 16 (MODE) -> 1 << MODE

void *simulate_sensor_task();
void *read_from_sensor_task();

#endif // SENSOR_H