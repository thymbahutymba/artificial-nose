#include "keyboard.h"

void get_keycodes(char *scan, char *ascii) {
    int k = readkey();
    *ascii = k & 0xFF;
    *scan = k >> 8;
}

void *keyboard_task(void *period) {
    struct timespec t;
    char scan, ascii;
    unsigned int i_key = 0; // last char inserted in buffer
    char path2save[BUFFER_SIZE];
    Task t_img = {-1, store_image_task, 20, 500};

    pthread_mutex_init(&mutex_keyboard, NULL);
    install_keyboard();

    pthread_mutex_lock(&mutex_keyboard);
    act_mode = WRITING;
    pthread_mutex_unlock(&mutex_keyboard);

    set_activation(&t, *((int *)period));

    while (!key[KEY_ESC]) {
        get_keycodes(&scan, &ascii);

        // Change actual mode when is pressed enter
        pthread_mutex_lock(&mutex_keyboard);
        if (scan == KEY_ENTER) {
            if (act_mode == SAVING) {
                sprintf(keyboard_buf, "%*s", BUFFER_SIZE - 1, " ");
                i_key = 0;
                pthread_cancel(t_img.id);
            } else {
                sprintf(path2save, "%s%s", PATH_I_NN, keyboard_buf);
                mkdir(path2save, 0755);
                task_create(&t_img);
            }
            act_mode = (act_mode == SAVING) ? WRITING : SAVING;
        };

        if (!act_mode && scan == KEY_BACKSPACE && i_key) {
            keyboard_buf[--i_key] = ' ';
        } else if (!act_mode && i_key < BUFFER_SIZE &&
                   ((scan >= KEY_A && scan <= KEY_9_PAD) || scan == KEY_MINUS ||
                    scan == KEY_STOP)) {
            keyboard_buf[i_key++] = ascii;
        }
        pthread_mutex_unlock(&mutex_keyboard);

        wait_for_activation(&t, *((int *)period));
    }

    return NULL;
}