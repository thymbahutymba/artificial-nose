#include "interface.h"
#include "keyboard.h"
#include "neural_network.h"
#include "ptask.h"
#include "sensor.h"

int main() {
    int index;

    Task task_table[] = {{-1, simulate_sensor_task, 25, 300},
                         {-1, graphic_task, 30, 20},
                         {-1, neural_network_task, 20, 1000},
                         /*{-1, store_image_task, 20, 500},*/
                         {-1, keyboard_task, 30, 15}};
    const int n_task = sizeof(task_table) / sizeof(Task);

    pthread_mutex_init(&mutex_data, NULL);
    pthread_mutex_init(&mutex_res, NULL);

    mkdir(PATH_I_NN, 0755);
    init_interface();

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    do {
    } while (pthread_join(task_table[n_task - 1].id, NULL));

    for (index = 0; index < n_task - 1; index++)
        pthread_cancel(task_table[index].id);

    allegro_exit();
    return 0;
}