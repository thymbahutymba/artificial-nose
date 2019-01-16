#include "keyboard.h"

/* Read the pressed key and split it */
void get_keycodes(char *scan, char *ascii) {
    int k = readkey();
    *ascii = k & 0xFF;
    *scan = k >> 8;
}

/* Check the pressed key and do accordingly. The key accepted in writing mode
 * are the letters from a to Z, numbers, minus and point, other characters are
 * ignored */
void handle_key(Task *t_img, char scan, char ascii, unsigned int *i_key) {
    char path2save[BUFFER_SIZE + strlen(PATH_I_NN)];

    pthread_mutex_lock(&mutex_keyboard);

    // Change actual mode when is pressed enter
    if (scan == KEY_ENTER) {
        if (cur_mode == SAVING) {
            // Clean the buffer, screen output and cancel the image storing task
            sprintf(keyboard_buf, "%*s", BUFFER_SIZE - 1, " ");
            *i_key = 0;

            // Cancel the storing thread and wait until the termination
            pthread_cancel(t_img->id);
            pthread_join(t_img->id, NULL);
        } else {
            // Create the folder for storing the image
            sprintf(path2save, "%s%s", PATH_I_NN, keyboard_buf);
            mkdir(path2save, 0755);

            // Start image storing task
            task_create(t_img);
        }
        // Switch current mode
        cur_mode = (cur_mode == SAVING) ? WRITING : SAVING;
    };

    // Delete letters when backspace is pressed and WRITING mode is active
    if (!cur_mode && scan == KEY_BACKSPACE && *i_key) {
        keyboard_buf[--(*i_key)] = '\0';
    } else if (!cur_mode && *i_key < BUFFER_SIZE &&
               ((scan >= KEY_A && scan <= KEY_9_PAD) || scan == KEY_MINUS ||
                scan == KEY_STOP)) {

        // Store the new letters when the WRITING mode is active
        keyboard_buf[(*i_key)++] = ascii;
        keyboard_buf[*i_key] = '\0';
    }
    pthread_mutex_unlock(&mutex_keyboard);
}

/* Periodic task to read input by keyboard, to switch mode between SAVING and
 * WRITING and to activate task to store the images. */
void *keyboard_task() {
    struct timespec t;      // Time refering the period
    struct timespec dl;     // Time refering the deadline
    char scan;              // Scan code of key pressed
    char ascii;             // Ascii of key pressed
    unsigned int i_key = 0; // last char inserted in buffer

    // Keyboard inizialization
    install_keyboard();

    // Starts in writing mode
    pthread_mutex_lock(&mutex_keyboard);
    cur_mode = WRITING;
    pthread_mutex_unlock(&mutex_keyboard);

    set_activation(&t, task_table[K_I].period);
    set_activation(&dl, task_table[K_I].period);

    while (1) {
        // Get the key pressed from keyboard
        get_keycodes(&scan, &ascii);

        // Reaction based on key that was pressed if different
        if (scan == KEY_ESC)
            return NULL;
        else
            handle_key(&task_table[SI_I], scan, ascii, &i_key);

        check_deadline(&dl, K_I);
        wait_for_activation(&t, &dl, task_table[K_I].period);
    }
}