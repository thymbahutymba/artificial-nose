#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "ptask.h"
#include "interface.h"
#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>

#define BUFFER_SIZE (25)
#define PATH_I_NN ("image_neural_network/")

typedef enum mode { WRITING, SAVING } mode;

char keyboard_buf[BUFFER_SIZE];
mode act_mode;

pthread_mutex_t mutex_keyboard;

void *keyboard_task(void *);

#endif // KEYBOARD_H