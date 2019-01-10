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

// Array of labels for which the nn is trained
#define LABELS ((const char *const[]){"A: %f", "B: %f", "C: %f", "D: %f"})
#define N_LAB (sizeof(LABELS) / sizeof(char *)) // Number of labels

#define FIXED_S (299) // Fixed size of image accepted by tensorflow model

// Number of elements contained into tensor
#define ARRAY_SIZE (FIXED_S * FIXED_S * CHANNELS)

// Max file size for load the tensorflow graph
#define MAX_FS (1 << 27)

#define MAX_CC (1 << 8) // Max value for each color channel

/* Data structure that contains all stuff allocated by tensorflow */
struct args {
    TF_Session *session;
    TF_Status *status;
    TF_SessionOptions *sess_opts;
    TF_Graph *graph;
    TF_Tensor *out_vals;
};

typedef short unsigned int img_t; // Type of value for each pixel of image

float *result;
pthread_mutex_t mutex_res;

void *neural_network_task(void *);

#endif // NEURAL_NETWORK_H