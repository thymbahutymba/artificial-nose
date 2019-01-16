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

#define MAX_FS (1 << 27) // Max file size for load the tensorflow graph
#define MAX_CC (1 << 5)  // Max value for each color channel

TF_Graph *graph;            // Graph associated to session
TF_Status *status;          // Result status of tensorflow execution
TF_Session *session;        //
TF_Tensor *out_vals;        // Tensor that contains results of execution
TF_Output input_op, output; // Input and output layers

typedef short unsigned int img_t; // Type of value for each pixel of image
unsigned char f_graph[MAX_FS];    // Array to contain the graph
float *result;                    // Results taken from neural network
pthread_mutex_t mutex_res;

extern Task task_table[]; // Link to task table initialized into main file

void *neural_network_task();

#endif // NEURAL_NETWORK_H