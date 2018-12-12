#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <tensorflow/c/c_api.h>
#include "keyboard.h"
#include "ptask.h"

void *neural_network_task(void *);

#endif // NEURAL_NETWORK_H