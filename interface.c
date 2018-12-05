#include "interface.h"

void init_interface() {
    allegro_init();
    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    install_keyboard();
}

void draw_background() {
    int i;
    char *legend_text[] = {"ESC: Exit from simulation",
                           "ENTER:", "LEFT:", "RIGHT: "};

    const int legend_element = sizeof(legend_text) / sizeof(char *);

    acquire_screen();

    // draw section for graph location
    rect(screen, GRAPH_X1, GRAPH_Y1, GRAPH_X2, GRAPH_Y2, BORDER_COLOR);
    // line(screen, GRAPH_X1, GRAPH_Y2 + G_SUBBOX, GRAPH_X2, GRAPH_Y2 +
    // G_SUBBOX, BORDER_COLOR);
    rect(screen, SUBBOX_X1, SUBBOX_Y1, SUBBOX_X2, SUBBOX_Y2, BORDER_COLOR);

    // draw section for image location
    rect(screen, IMAGE_X1, IMAGE_Y1, IMAGE_X2, IMAGE_Y2, BORDER_COLOR);

    // draw section for result location
    rect(screen, RESULT_X1, RESULT_Y1, RESULT_X2, RESULT_Y2, BORDER_COLOR);

    // draw section for legend location
    rect(screen, LEGEND_X1, LEGEND_Y1, LEGEND_X2, LEGEND_Y2, BORDER_COLOR);
    textout_ex(screen, font, "LEGEND", LTEXT_X, LTEXT_Y, MAIN_COLOR, BKG_COLOR);

    for (i = 0; i < legend_element; i++)
        textout_ex(screen, font, legend_text[i], LTEXT_X,
                   LTEXT_Y + LINE_SPACE * (i + 1), TEXT_COLOR, BKG_COLOR);

    textout_ex(screen, font, "Current value:", SXT_S, SYT_SCURRENT, TEXT_COLOR,
               BKG_COLOR);

    release_screen();
}

void draw_graphic(unsigned int *last_draw) {
    // location of x axis on the screen
    const unsigned int base = GRAPH_Y1 - INTERNAL_MARGIN;

    // height of portion of screen where will be placed the graph
    const unsigned int g_height = GRAPH_HEIGHT - 2 * INTERNAL_MARGIN;

    BITMAP *bmp;

    pthread_mutex_lock(&mutex_data);
    for (; *last_draw != (r_data.top + GRAPH_ELEMENT - 2) % GRAPH_ELEMENT;
         *last_draw = (++(*last_draw)) % GRAPH_ELEMENT) {

        const unsigned int norm_y1 =
            (float)r_data.elem[*last_draw] / UPPER_LIMIT * g_height;
        const unsigned int norm_y2 =
            (float)r_data.elem[*last_draw + 1] / UPPER_LIMIT * g_height;

        acquire_screen();
        fastline(screen, r_data.x_point[*last_draw], base - norm_y1,
                 r_data.x_point[*last_draw + 1], base - norm_y2, TEXT_COLOR);
        release_screen();
    }
    pthread_mutex_unlock(&mutex_data);

    if (*last_draw == GRAPH_ELEMENT - 1) {
        acquire_screen();
        bmp = create_sub_bitmap(
            screen, GRAPH_X1 + INTERNAL_MARGIN, GRAPH_Y2 + INTERNAL_MARGIN,
            GRAPH_WIDTH - INTERNAL_MARGIN, GRAPH_HEIGHT - INTERNAL_MARGIN);
        clear_bitmap(bmp);
        // destroy_bitmap(bmp);
        release_screen();

        *last_draw = 0;
    }
}

void draw_information() {
    uint16_t v_current = 0; // current value
    char s[5];              // string to be printed

    pthread_mutex_lock(&mutex_data);
    v_current = r_data.elem[(r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT];
    pthread_mutex_unlock(&mutex_data);

    acquire_screen();
    sprintf(s, "%i", v_current);

    textout_ex(screen, font, "     ", SXT_D, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_D, SYT_SCURRENT, TEXT_COLOR, 0);

    release_screen();
}

void draw_image(unsigned int *last_draw) {

    const unsigned int e_height =
        (unsigned int)(IMAGE_HEIGHT - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT;
    const unsigned int e_width = IMAGE_WIDTH - INTERNAL_MARGIN * 2;
    int x, y;
    int size = (GRAPH_ELEMENT - 1) * e_height;
    BITMAP *image_bmp = create_bitmap(e_width, size);
    BITMAP *row_bmp;

    acquire_screen();
    pthread_mutex_lock(&mutex_data);

    for (; *last_draw != (r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT;
         *last_draw = (++(*last_draw)) % GRAPH_ELEMENT) {
        x = IMAGE_X2 + INTERNAL_MARGIN;
        y = IMAGE_Y2 + INTERNAL_MARGIN;

        blit(screen, image_bmp, x, y, 0, 0, e_width, size);

        blit(image_bmp, screen, 0, 0, x, y + e_height, e_width, size);

        row_bmp = create_sub_bitmap(screen, x, y, e_width, e_height - 1);
        clear_bitmap(row_bmp);

        rectfill(screen, x, y, x + e_width - 1, y + e_height - 1,
                 r_data.elem[*last_draw]);
    }
    pthread_mutex_unlock(&mutex_data);
    release_screen();
}

void save_image(index_image) {
    PALETTE pal;                        // color palette
    char str[80];                       // name of file to save
    int x = IMAGE_X2 + INTERNAL_MARGIN; // x top corner where image starts
    int y = IMAGE_Y2 + INTERNAL_MARGIN; // y top corner where image starts

    const unsigned int e_height =
        (unsigned int)(IMAGE_HEIGHT - INTERNAL_MARGIN * 2) /
        GRAPH_ELEMENT; // height of each element of the queue

    const unsigned int h = GRAPH_ELEMENT * e_height; // height of image to save

    const unsigned int w =
        IMAGE_WIDTH - INTERNAL_MARGIN * 2; // width in pixel of image
    acquire_screen();
    BITMAP *image_bmp = create_sub_bitmap(screen, x, y, w, h);
    get_palette(pal);

    // name of image to be saved and save it to bmp file
    sprintf(str, "/tmp/image_neural_network/image_%08i.bmp", index_image);
    save_bmp(str, image_bmp, pal);
    release_screen();

}

void *store_image_task(void *period) {
    struct timespec t;

    unsigned int index_image = 0; // counter of image saved

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, *(int *)period);

    while (1) {
        save_image(index_image++);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, *(int *)period);
    }
}

void *graphic_task(void *period) {
    struct timespec t;
    unsigned int ld_image = 0, ld_graph = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, *(int *)period);

    draw_background();

    while (1) {
        draw_graphic(&ld_graph);
        draw_information();
        draw_image(&ld_image);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, *(int *)period);
    }

    allegro_exit();
}