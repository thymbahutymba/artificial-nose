#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <tensorflow/c/c_api.h>
#include "keyboard.h"
#include "ptask.h"
#include "interface.h"

#define GRAPH_NAME ("retrained_graph.pb")
#define LABELS_NAME ("retrained_labels.txt")
#define IN_NAME ("Placeholder")
#define OUT_NAME ("final_result")

#define ARRAY_SIZE (EL_W * ACT_IMG_H * CHANNELS)

// Change this or array data with less useless name
typedef unsigned int data_t;

void *neural_network_task(void *);

#endif // NEURAL_NETWORK_H