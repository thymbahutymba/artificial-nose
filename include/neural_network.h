#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "interface.h"
#include "keyboard.h"
#include "ptask.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <tensorflow/c/c_api.h>

#define GRAPH_NAME ("graph.pb")    // File in which the graph is stored
#define LABELS_NAME ("labels.txt") // File in which the labels are stored
#define IN_NAME ("Placeholder")    // Name of input layer (default)
#define OUT_NAME ("final_result")  // Name of output layer (default)

#define N_LAB (4)
#define LABELS ((const char *const[N_LAB]){"A: %f", "B: %f", "C: %f", "D: %f"})

#define FIXED_S (299) // Fixed size of image accepted by tensorflow model

// Number of elements contained into tensor
#define ARRAY_SIZE (FIXED_S * FIXED_S * CHANNELS)

#define MAX_CC (1 << 8) // Max value for each color channel

typedef short unsigned int img_t; // Type of value for each pixel of image

float *result;
pthread_mutex_t mutex_res;

void *neural_network_task(void *);

#endif // NEURAL_NETWORK_H