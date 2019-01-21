#include "interface.h"
#include "keyboard.h"
#include "neural_network.h"
#include "ptask.h"
#include "sensor.h"

// Initialization of task table
Task task_table[] = {{-1, store_image_task, 20, 1000, 0, 0},
                     {-1, read_from_sensor_task, 30, 500, 0, 0},
                     {-1, graphic_task, 30, 100, 0, 0},
                     {-1, neural_network_task, 25, 2000, 0, 0},
                     {-1, keyboard_task, 30, 100, 0, 0}};
pthread_mutex_t mutex_tt;

int main() {
    size_t index;

    // Mutexes initialization
    pthread_mutex_init(&mutex_data, NULL);
    pthread_mutex_init(&mutex_res, NULL);
    pthread_mutex_init(&mutex_keyboard, NULL);
    pthread_mutex_init(&mutex_tt, NULL);

    mkdir(PATH_I_NN, 0755); // Creation of directory in which images are saved
    init_interface();       // Initialization of allegro interface

    // Start all task without the store image task
    for (index = 1; index < N_TASK; index++)
        task_create(&task_table[index]);

    // Wait until the keyboard task ends due to pressing key ESC
    do {
    } while (pthread_join(task_table[N_TASK - 1].id, NULL));

    // Cancel all other threads running
    for (index = 1; index < N_TASK - 1; index++)
        pthread_cancel(task_table[index].id);

    // Wait for the termination of all other threads
    for (index = 1; index < N_TASK - 1; index++)
        pthread_join(task_table[index].id, NULL);

    return 0;
}

END_OF_MAIN()