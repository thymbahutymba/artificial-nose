#include "interface.h"
#include "keyboard.h"
#include "neural_network.h"
#include "ptask.h"
#include "sensor.h"

int main() {
    size_t index;

    Task task_table[] = {{-1, simulate_sensor_task, 25, 300},
                         {-1, graphic_task, 30, 20},
                         {-1, neural_network_task, 20, 1000},
                         {-1, keyboard_task, 30, 15}};
    const size_t n_task = sizeof(task_table) / sizeof(Task);

    pthread_mutex_init(&mutex_data, NULL);
    pthread_mutex_init(&mutex_res, NULL);

    mkdir(PATH_I_NN, 0755);
    init_interface();

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    // Wait until the keyboard task ends due to pressing key ESC
    do {
    } while (pthread_join(task_table[n_task - 1].id, NULL));

    // Cancel all other threads running
    for (index = 0; index < n_task - 1; index++)
        pthread_cancel(task_table[index].id);

    // Wait for the termination of all other threads
    for (index = 0; index < n_task - 1; index++)
        pthread_join(task_table[index].id, NULL);

    allegro_exit();
    return 0;
}