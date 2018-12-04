#ifndef SENSOR_H
#define SENSOR_H

#include "interface.h"
#include "ptask.h"

#define RANGE (128)
#define BOTTOM_LIMIT (0 + RANGE)
#define UPPER_LIMIT ((65536) - RANGE)

void *simulate_sensor_task();

#endif // SENSOR_H