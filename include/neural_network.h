#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "interface.h"
#include "keyboard.h"
#include "ptask.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <tensorflow/c/c_api.h>

#define GRAPH_NAME ("graph.pb")
#define LABELS_NAME ("labels.txt")
#define IN_NAME ("Placeholder")
#define OUT_NAME ("final_result")

#define N_LAB (3)
#define LABELS ((const char *const [N_LAB]){"Aglio:   %f", \
                                            "Cipolla: %f", \
                                            "Uova:    %f" })

#define ARRAY_SIZE (EL_W * ACT_IMG_H * CHANNELS)

typedef short unsigned int img_t;
typedef float tfdat_t; // Tensorflow data type for the array passed to tensor

float *result;
pthread_mutex_t mutex_res;

void *neural_network_task(void *);

#endif // NEURAL_NETWORK_H