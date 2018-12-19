/*******************************************************************************
 * This file contains functions needed for drawing into screen
 ******************************************************************************/

#include "interface.h"

/* Initialization of allegro and setting color mode to RGBA */
void init_interface() {
    PALETTE palette;

    allegro_init();
    set_color_depth(COLOR_MODE);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    font = load_font("fonts/hackBold10.pcx", palette, NULL);
}

/* Prints of all background that will not be changed during program execution */
void draw_background() {
    int i;

    // Legend that display which are the possible interaction with program
    char *legend_text[] = {
        "ESC: Exit from simulation",
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
    textout_ex(screen, font, "Current value:  CO2:     tVOC:", SXT_S,
               SYT_SCURRENT, TEXT_COLOR, BKG_COLOR);

    release_screen();
}

/* Normalization of coordinates for graph drawing */
void norm_cord(int index, unsigned int *n_co2, unsigned int *n_tvoc) {
    // Height of portion of screen where will be placed the graph
    const unsigned int g_height = GRAPH_HEIGHT - 2 * INTERNAL_MARGIN;

    /* Normalization of CO2 value among the height of the section used for
     * the graphs. */
    *n_co2 = (float)r_data.co2[index] / UPPER_LIMIT * g_height;

    /* Normalization of tVOC value among the height of the section used for
     * the graphs. */
    *n_tvoc = (float)r_data.tvoc[index] / UPPER_LIMIT * g_height;
}

/* Clearing the section containing the graphs */
void clear_graph() {
    BITMAP *bmp; // Bitmap for clearing image on bottom

    acquire_screen();

    // Create reference to graph zone
    bmp = create_sub_bitmap(
        screen, GRAPH_X1 + INTERNAL_MARGIN, GRAPH_Y2 + INTERNAL_MARGIN,
        GRAPH_WIDTH - INTERNAL_MARGIN, GRAPH_HEIGHT - INTERNAL_MARGIN);

    // Clean graph section for drawing new graph from start
    clear_bitmap(bmp);
    release_screen();
}

/* Drawing of both graphs tVOC and CO2 that represents the last GRAPH_ELEMENT
 * values that were sampled by sensor. */
void draw_graph(unsigned int *last_draw) {
    // Location of x axis on the screen
    const unsigned int base = GRAPH_Y1 - INTERNAL_MARGIN;
    unsigned int n_co2_1, n_tvoc_1; // normalized value of line start point
    unsigned int n_co2_2, n_tvoc_2; // normalized value of line end point

    pthread_mutex_lock(&mutex_data);

    // Prints of the portions of graphs that is not be drawn yet
    for (; *last_draw != (r_data.top + GRAPH_ELEMENT - 2) % GRAPH_ELEMENT;
         *last_draw = (*last_draw + 1) % GRAPH_ELEMENT) {

        /* Compute the normalized values. These values are two in order to draw
         * a line that joins both points. */
        norm_cord(*last_draw, &n_co2_1, &n_tvoc_1);
        norm_cord(*last_draw + 1, &n_co2_2, &n_tvoc_2);

        acquire_screen();
        /* Drawing of the line for CO2 graph that joins the two points that
         * have been considered */
        fastline(screen, r_data.x_point[*last_draw], base - n_co2_1,
                 r_data.x_point[*last_draw + 1], base - n_co2_2, GRAPH1_COLOR);

        /* Drawing of the line for tVOC graph that joins the two points that
         * have been considered */
        fastline(screen, r_data.x_point[*last_draw], base - n_tvoc_1,
                 r_data.x_point[*last_draw + 1], base - n_tvoc_2, GRAPH2_COLOR);
        release_screen();
    }
    pthread_mutex_unlock(&mutex_data);

    // Check if the queue has been filled and then clear the graph section
    if (*last_draw == GRAPH_ELEMENT - 1) {
        clear_graph();
        // Update of the index containing the last element drawn
        *last_draw = 0;
    }
}

/* Print on screen the current values readed from sensor */
void draw_information() {
    uint16_t c_CO2 = 0, c_tVOC = 0; // current values for CO2 and tVOC
    char s[5];                      // string to be printed

    // Get last values of CO2 and tVOC
    pthread_mutex_lock(&mutex_data);
    c_CO2 = r_data.co2[(r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT];
    c_tVOC = r_data.tvoc[(r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT];
    pthread_mutex_unlock(&mutex_data);

    acquire_screen();

    // Print on screen the CO2 value
    sprintf(s, "%i", c_CO2);
    textout_ex(screen, font, "    ", SXT_CO2, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_CO2, SYT_SCURRENT, TEXT_COLOR, 0);

    // Print on screen the tVOC value
    sprintf(s, "%i", c_tVOC);
    textout_ex(screen, font, "     ", SXT_TVOC, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_TVOC, SYT_SCURRENT, TEXT_COLOR, 0);

    release_screen();
}

/* Shift image one line at bottom */
void shift_to_bottom() {
    int size = (GRAPH_ELEMENT - 1) * (int)EL_H; // Image height without last row
    BITMAP *img_bmp = create_bitmap(EL_W, size); // image without last row
    BITMAP *row_bmp;

    // Store image without last row
    blit(screen, img_bmp, IMG_XT, IMG_YT, 0, 0, EL_W, size);

    // Replace image with stored image that not contains anymore last row
    blit(img_bmp, screen, 0, 0, IMG_XT, IMG_YT + (int)EL_H, EL_W, size);

    // clear the firt line of the image
    row_bmp = create_sub_bitmap(screen, IMG_XT, IMG_YT, EL_W, (int)EL_H - 1);
    clear_bitmap(row_bmp);
}

/* Displays the last values ​​not yet printed on the screen. */
void draw_image(unsigned int *last_draw) {
    acquire_screen();
    pthread_mutex_lock(&mutex_data);

    // Draws the elements that are not drawn yet
    for (; *last_draw != (r_data.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT;
         *last_draw = (*last_draw + 1) % GRAPH_ELEMENT) {

        // Shift image one line at bottom
        shift_to_bottom();

        // Draw new rectangle colored with the new value
        rectfill(screen, IMG_XT, IMG_YT, IMG_XT + (EL_W - 1) / 2,
                 IMG_YT + (int)EL_H - 1, r_data.co2[*last_draw]);
        rectfill(screen, IMG_XT + (EL_W - 1) / 2, IMG_YT, IMG_XT + EL_W - 1,
                 IMG_YT + (int)EL_H - 1, r_data.tvoc[*last_draw]);
    }
    pthread_mutex_unlock(&mutex_data);
    release_screen();
}

/* Save image to file, the directory is the one previously created */
void save_image(int index_image) {
    PALETTE pal;  // color palette
    char str[80]; // name of file to save

    acquire_screen();

    BITMAP *image_bmp =
        create_sub_bitmap(screen, IMG_XT, IMG_YT, EL_W, ACT_IMG_H);

    get_palette(pal);

    // File name of image to be saved
    pthread_mutex_lock(&mutex_keyboard);
    sprintf(str, "%s%s/image_%04i.bmp", PATH_I_NN, keyboard_buf, index_image);
    pthread_mutex_unlock(&mutex_keyboard);

    save_bmp(str, image_bmp, pal);

    release_screen();
}

/* Draw current keyboard mode and text acquired from it */
void draw_text() {
    char *txt_mode[] = {"WRITING: ", "SAVING:  "};
    // Offset among X axis for printing text input
    int x_off = strlen(txt_mode[0]) * 10;

    pthread_mutex_lock(&mutex_keyboard);

    // Print current mode
    textout_ex(screen, font, txt_mode[cur_mode], TEXT_X1, TEXT_Y1, MAIN_COLOR,
               BKG_COLOR);

    // Print text acquired from keyboard
    textout_ex(screen, font, keyboard_buf, TEXT_X1 + x_off, TEXT_Y1, TEXT_COLOR,
               BKG_COLOR);
    pthread_mutex_unlock(&mutex_keyboard);
}

void draw_results() {
    size_t i;
    char msg[BUFFER_SIZE];

    textout_ex(screen, font, "RESULTS", RTEXT_X, RTEXT_Y, MAIN_COLOR,
               BKG_COLOR);

    pthread_mutex_lock(&mutex_res);

    for (i = 0; i < N_LAB; ++i) {
        if (result == NULL)
            sprintf(msg, LABELS[i], 0);
        else
            sprintf(msg, LABELS[i], result[i]);

        textout_ex(screen, font, msg, RTEXT_X, RTEXT_Y + LINE_SPACE * (i + 1),
                   TEXT_COLOR, BKG_COLOR);
    }
    pthread_mutex_unlock(&mutex_res);
}

/* Periodic task manually activated for images storing in specific directory
 * created in writing mode. */
void *store_image_task(void *period) {
    struct timespec t;
    unsigned int index_image = 0; // Counter of saved images

    set_activation(&t, *((int *)period));

    while (1) {
        // Save image in its directory
        save_image(index_image++);
        wait_for_activation(&t, *((int *)period));
    }
}

/* Periodic task for the drawing of image, graph, information sampled from
 * sensor and text acquried from keyboard */
void *graphic_task(void *period) {
    struct timespec t;
    unsigned int ld_image = 0; // Index of last drawn element into image section
    unsigned int ld_graph = 0; // Index of last drawn element into graph section

    set_activation(&t, *((int *)period));

    draw_background();

    while (1) {
        // Prints into graph the element sampled that are not printed yet
        draw_graph(&ld_graph);

        // Prints current informations sampled from sensor
        draw_information();

        // Prints into image the element sampled that are not printed yet
        draw_image(&ld_image);

        // Prints text acquired from keyboard
        draw_text();

        draw_results();
        wait_for_activation(&t, *((int *)period));
    }

    // allegro_exit();
}