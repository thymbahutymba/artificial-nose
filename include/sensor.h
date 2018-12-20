#ifndef SENSOR_H
#define SENSOR_H

#include "interface.h"
#include "ptask.h"
#include <termios.h>

// Range between which the reference value can fluctuate
#define RANGE (512) // 1 << 11
#define BOTTOM_LIMIT (0)
#define UPPER_LIMIT (65536) // 1 << 16 (MODE) -> 1 << MODE

// Reference values ​​for each class for which the neural network has been trained
#ifdef A
#define R_CO2 (50000)
#define R_TVOC (20000)

#elif B
#define R_CO2 (60000)
#define R_TVOC (15000)

#elif C
#define R_CO2 (40000)
#define R_TVOC (5000)

#else
#define R_CO2 (15000)
#define R_TVOC (60000)
#endif

void *simulate_sensor_task();
void *read_from_sensor_task();

#endif // SENSOR_H