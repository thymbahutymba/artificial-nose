#include "interface.h"
#include "keyboard.h"
#include "neural_network.h"
#include "ptask.h"
#include "sensor.h"

Task task_table[] = {{-1, store_image_task, 20, 1000, 0},
                     {-1, read_from_sensor_task, 30, 500, 0},
                     {-1, graphic_task, 30, 100, 0},
                     {-1, neural_network_task, 25, 2000, 0},
                     {-1, keyboard_task, 30, 80, 0}};

int main() {
    size_t index;
    const size_t n_task = sizeof(task_table) / sizeof(Task);

    pthread_mutex_init(&mutex_data, NULL);
    pthread_mutex_init(&mutex_res, NULL);
    pthread_mutex_init(&mutex_keyboard, NULL);

    mkdir(PATH_I_NN, 0755);
    init_interface();

    // Start all task without the store image task
    for (index = 1; index < n_task; index++)
        task_create(&task_table[index]);

    // Wait until the keyboard task ends due to pressing key ESC
    do {
    } while (pthread_join(task_table[n_task - 1].id, NULL));

    // Cancel all other threads running
    for (index = 1; index < n_task - 1; index++)
        pthread_cancel(task_table[index].id);

    // Wait for the termination of all other threads
    for (index = 1; index < n_task - 1; index++)
        pthread_join(task_table[index].id, NULL);

    for (index = 0; index < n_task; index++)
        printf("%li ", task_table[index].dmiss);

    return 0;
}

END_OF_MAIN()