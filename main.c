#include "interface.h"
#include "ptask.h"
#include "sensor.h"

#include <sys/stat.h>

void get_keycodes(char *scan) {
    int k = readkey();
    *scan = k >> 8;
}

int main() {
    int index;
    Task task_table[] = {
        {-1, simulate_sensor_task, 25},
        {-1, graphic_task, 20},
    };
    const int n_task = sizeof(task_table) / sizeof(Task);

    pthread_mutex_init(&mutex_data, NULL);

    mkdir("/tmp/image_neural_network", 0744);

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    do {
    } while (!key[KEY_ESC]);

    for (index = 0; index < n_task; index++)
        pthread_cancel(task_table[index].id);

    return 0;
}