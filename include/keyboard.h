#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "interface.h"
#include "ptask.h"
#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>

#define BUFFER_SIZE (25) // Size of the buffer to read the input by keyboard
#define PATH_I_NN ("image_neural_network/") // Path where to save the images

typedef enum mode { WRITING, SAVING } mode;

char keyboard_buf[BUFFER_SIZE]; // Buffer to read the input by keyboard
mode cur_mode;                  // Current application mode

pthread_mutex_t mutex_keyboard; // Mutex for keyboard_buf and cur_mode

void *keyboard_task(void *);

#endif // KEYBOARD_H