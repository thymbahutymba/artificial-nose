#include "keyboard.h"

// Read the pressed key and split it
void get_keycodes(char *scan, char *ascii) {
    int k = readkey();
    *ascii = k & 0xFF;
    *scan = k >> 8;
}

// Check the pressed key and do accordingly
void check_input_key(char scan, char ascii) {
    char path2save[BUFFER_SIZE];
    unsigned int i_key = 0; // last char inserted in buffer
    Task t_img = {-1, store_image_task, 20, 500};

    pthread_mutex_lock(&mutex_keyboard);
    // Change actual mode when is pressed enter
    if (scan == KEY_ENTER) {
        // If current mode is SAVING and press ENTER, clean buffer and kill task
        // to store image
        if (cur_mode == SAVING) {
            sprintf(keyboard_buf, "%*s", BUFFER_SIZE - 1, " ");
            i_key = 0;
            pthread_cancel(t_img.id);
        } else // If current mode is WRITING and press ENTER, create folder and
               // start task to store image
        {
            sprintf(path2save, "%s%s", PATH_I_NN, keyboard_buf);
            mkdir(path2save, 0755);
            task_create(&t_img);
        }
        // Switch current mode
        cur_mode = (cur_mode == SAVING) ? WRITING : SAVING;
    };

    // If current mode is WRITING and press BACKSPACE, delete the last letter
    if (!cur_mode && scan == KEY_BACKSPACE && i_key) {
        keyboard_buf[--i_key] = ' ';
    } else if (!cur_mode && i_key < BUFFER_SIZE &&
               ((scan >= KEY_A && scan <= KEY_9_PAD) || scan == KEY_MINUS ||
                scan == KEY_STOP)) {
        keyboard_buf[i_key++] = ascii;
    }
    pthread_mutex_unlock(&mutex_keyboard);
}

/* Periodic task to read input by keyboard, to switch mode between SAVING and
 * WRITING and to activate task to store the images. */
void *keyboard_task(void *period) {
    struct timespec t;
    char scan, ascii;

    pthread_mutex_init(&mutex_keyboard, NULL);
    install_keyboard();

    pthread_mutex_lock(&mutex_keyboard);
    cur_mode = WRITING;
    pthread_mutex_unlock(&mutex_keyboard);

    set_activation(&t, *((int *)period));

    while (!key[KEY_ESC]) {
        get_keycodes(&scan, &ascii);

        check_input_key(scan, ascii);

        wait_for_activation(&t, *((int *)period));
    }

    return NULL;
}