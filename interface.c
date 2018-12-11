/*******************************************************************************
 * This file contains functions needed for drawing into screen
 ******************************************************************************/

#include "interface.h"

void init_interface() {
    allegro_init();
    set_color_depth(COLOR_MODE);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
}

/*
 * Prints of all background that will not be changed during program execution
 */
void draw_background() {
    int i;

    // Legend that display which are the possible interaction with program
    char *legend_text[] = {"ESC: Exit from simulation",
                           "ENTER: switch between mode",
                           };

    const int legend_element = sizeof(legend_text) / sizeof(char *);

    acquire_screen();

    // Draws rectangle that delimits the graphs section
    rect(screen, GRAPH_X1, GRAPH_Y1, GRAPH_X2, GRAPH_Y2, BORDER_COLOR);

    // Draws rectangle that delimits the sub box section
    rect(screen, SUBBOX_X1, SUBBOX_Y1, SUBBOX_X2, SUBBOX_Y2, BORDER_COLOR);

    // Draws rectangle that delimits the image section
    rect(screen, IMAGE_X1, IMAGE_Y1, IMAGE_X2, IMAGE_Y2, BORDER_COLOR);

    // Draws rectangle that delimits the result section
    rect(screen, RESULT_X1, RESULT_Y1, RESULT_X2, RESULT_Y2, BORDER_COLOR);

    // Draws rectangle that delimits the legend section
    rect(screen, LEGEND_X1, LEGEND_Y1, LEGEND_X2, LEGEND_Y2, BORDER_COLOR);
    textout_ex(screen, font, "LEGEND", LTEXT_X, LTEXT_Y, MAIN_COLOR, BKG_COLOR);

    // Draws rectangle that delimits the keyboard input
    rect(screen, INPUT_X1, INPUT_Y1, INPUT_X2, INPUT_Y2, BORDER_COLOR);

    // Prints each row that is contained in legend
    for (i = 0; i < legend_element; i++)
        textout_ex(screen, font, legend_text[i], LTEXT_X,
                   LTEXT_Y + LINE_SPACE * (i + 1), TEXT_COLOR, BKG_COLOR);

    // Prints the text where will be displayed CO2 and tVOC values
    textout_ex(screen, font, "Current value:    CO2:        tVOC:", SXT_S,
               SYT_SCURRENT, TEXT_COLOR, BKG_COLOR);

    release_screen();
}

/*
 * Drawing of both graphs tVOC and CO2 that represents the last GRAPH_ELEMENT
 * values that were sampled by sensor
 */
void draw_graphic(unsigned int *last_draw) {
    // Location of x axis on the screen
    const unsigned int base = GRAPH_Y1 - INTERNAL_MARGIN;

    // Height of portion of screen where will be placed the graph
    const unsigned int g_height = GRAPH_HEIGHT - 2 * INTERNAL_MARGIN;

    BITMAP *bmp;

    pthread_mutex_lock(&mutex_data);

    // Prints of the portions of graphs that is not be drawn yet
    for (; *last_draw != (r_data.top + GRAPH_ELEMENT - 2) % GRAPH_ELEMENT;
         *last_draw = (++(*last_draw)) % GRAPH_ELEMENT) {

        /*
         * Normalization of CO2 value among the height of the section used for
         * the graphs. The normalized values ​​are two in order to draw a
         * line that joins both points.
         */
        const unsigned int norm_co2_1 =
            (float)r_data.co2[*last_draw] / UPPER_LIMIT * g_height;
        const unsigned int norm_co2_2 =
            (float)r_data.co2[*last_draw + 1] / UPPER_LIMIT * g_height;

        /*
         * Normalization of tVOC value among the height of the section used for
         * the graphs. The normalized values ​​are two in order to draw a
         * line that joins both points.
         */
        const unsigned int norm_tvoc_1 =
            (float)r_data.tvoc[*last_draw] / UPPER_LIMIT * g_height;
        const unsigned int norm_tvoc_2 =
            (float)r_data.tvoc[*last_draw + 1] / UPPER_LIMIT * g_height;

        acquire_screen();
        // Drawing of the line for CO2 graph that joins the two points that
        // have been considered
        fastline(screen, r_data.x_point[*last_draw], base - norm_co2_1,
                 r_data.x_point[*last_draw + 1], base - norm_co2_2,
                 GRAPH1_COLOR);

        // Drawing of the line for tVOC graph that joins the two points that
        // have been considered
        fastline(screen, r_data.x_point[*last_draw], base - norm_tvoc_1,
                 r_data.x_point[*last_draw + 1], base - norm_tvoc_2,
                 GRAPH2_COLOR);
        release_screen();
    }
    pthread_mutex_unlock(&mutex_data);

    // Clearing the section containing the graphs if the queue has been filled
    if (*last_draw == GRAPH_ELEMENT - 1) {
        acquire_screen();
        bmp = create_sub_bitmap(
            screen, GRAPH_X1 + INTERNAL_MARGIN, GRAPH_Y2 + INTERNAL_MARGIN,
            GRAPH_WIDTH - INTERNAL_MARGIN, GRAPH_HEIGHT - INTERNAL_MARGIN);
        clear_bitmap(bmp);
        release_screen();

        // Update of the index containing the last element drawn
        *last_draw = 0;
    }
}

/*
 * TODO: Add an explanation of the function
 */
void draw_information() {
    uint16_t c_CO2 = 0,
             c_tVOC = 0; // current values for CO2 and tVOC
    char s[5];           // string to be printed

    // Get last values of CO2 and tVOC
    pthread_mutex_lock(&mutex_data);
    c_CO2 = r_data.co2[(r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT];
    c_tVOC = r_data.tvoc[(r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT];
    pthread_mutex_unlock(&mutex_data);

    acquire_screen();

    // Print of last CO2 value
    sprintf(s, "%i", c_CO2);
    textout_ex(screen, font, "     ", SXT_CO2, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_CO2, SYT_SCURRENT, TEXT_COLOR, 0);

    // Print of last CO2 value
    sprintf(s, "%i", c_tVOC);
    textout_ex(screen, font, "     ", SXT_TVOC, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_TVOC, SYT_SCURRENT, TEXT_COLOR, 0);

    release_screen();
}

/*
 * TODO: Add an explanation of the function
 */
void draw_image(unsigned int *last_draw) {

    const unsigned int e_height =
        (unsigned int)(IMAGE_HEIGHT - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT;
    const unsigned int e_width = IMAGE_WIDTH - INTERNAL_MARGIN * 2;
    int x, y;
    uint32_t color;
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

        color = ((uint32_t)(r_data.co2[*last_draw]) << 16) |
                r_data.tvoc[*last_draw];

        rectfill(screen, x, y, x + e_width - 1, y + e_height - 1, color);
    }
    pthread_mutex_unlock(&mutex_data);
    release_screen();
}

void save_image(int index_image) {
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

    set_activation(&t, *((int *)period));

    while (1) {
        save_image(index_image++);

        wait_for_activation(&t, *((int *)period));
    }
}

void draw_text() {
    char *txt_mode[] = {"WRITING: ", "SAVING:  "};
    pthread_mutex_lock(&mutex_keyboard);
    textout_ex(screen, font, txt_mode[act_mode], TEXT_X1, TEXT_Y1, MAIN_COLOR,
               BKG_COLOR);
    textout_ex(screen, font, keyboard_buf, TEXT_X1 + 80, TEXT_Y1, TEXT_COLOR,
               BKG_COLOR);
    pthread_mutex_unlock(&mutex_keyboard);
}

void *graphic_task(void *period) {
    struct timespec t;
    unsigned int ld_image = 0;
    unsigned int ld_graph = 0;

    set_activation(&t, *((int *)period));

    draw_background();

    while (1) {
        draw_graphic(&ld_graph);
        draw_information();
        draw_image(&ld_image);
        draw_text();

        wait_for_activation(&t, *((int *)period));
    }

    allegro_exit();
}