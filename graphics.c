#include "graphics.h"
#include <allegro.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOTTOM_LIMIT 0
#define UPPER_LIMIT 255

// TODO: move to graphic.h
typedef struct {
    unsigned int top, first;
    int x_point[GRAPH_ELEMENT]; // CHANGE NAME WITH A MORE USEFUL WORD
    unsigned int elem[GRAPH_ELEMENT];
} Queue;

Queue graph;
pthread_mutex_t mutex_graph;

typedef struct {
    pthread_t id;
    void *f;
    int priority;
} Task;

void init_interface() {
    allegro_init();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    set_color_depth(16);
    install_keyboard();
}

void draw_background() {
    int i;
    char *legend_text[] = {"ESC: Exit from simulation",
                           "ENTER:",
                           "LEFT:",
                           "RIGHT: "};

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
    textout_ex(screen, font, "LEGEND", LTEXT_X, LTEXT_Y, TITLE_COLOR, 0);

    for (i = 0; i < legend_element; i++)
        textout_ex(screen, font, legend_text[i], LTEXT_X,
                   LTEXT_Y + LINE_SPACE * (i + 1), TEXT_COLOR, 0);

    textout_ex(screen, font, "Current value:", SXT_S, SYT_SCURRENT, TEXT_COLOR,
               0);

    release_screen();
}

void draw_graphic(int *last_draw) {
    int i, j;
    const int offset =
        (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT);

    BITMAP *bmp;

    pthread_mutex_lock(&mutex_graph);
    for (*last_draw;
         *last_draw != (graph.top + GRAPH_ELEMENT - 2) % GRAPH_ELEMENT;
         *last_draw = ++(*last_draw) % GRAPH_ELEMENT) {
        acquire_screen();
        fastline(screen, graph.x_point[*last_draw],
                 GRAPH_Y1 - graph.elem[*last_draw],
                 graph.x_point[*last_draw + 1],
                 GRAPH_Y1 - graph.elem[*last_draw + 1], TEXT_COLOR);
        release_screen();
    }
    pthread_mutex_unlock(&mutex_graph);

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

void time_add_ms(struct timespec *t, int ms) {
    t->tv_sec += ms / 1000;
    t->tv_nsec += (ms % 1000) * 1000000;
    if (t->tv_nsec > 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

/* Initialization of the queue for values that will be sampled */
void init_queue() {
    int i;
    const int offset =
        (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) /
              GRAPH_ELEMENT); // distance between values along abscissa

    pthread_mutex_lock(&mutex_graph);
    graph.top = graph.first = 0;

    // inizialization of abscissa values
    for (i = 0; i < GRAPH_ELEMENT; ++i)
        graph.x_point[i] = GRAPH_X1 + INTERNAL_MARGIN + i * offset;

    pthread_mutex_unlock(&mutex_graph);
}

void *simulate_sensor_task() {
    struct timespec t;
    int period = 150;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    init_queue();

    while (1) {
        pthread_mutex_lock(&mutex_graph);
        graph.elem[graph.top] =
            rand() % (UPPER_LIMIT - BOTTOM_LIMIT + 1) +
            BOTTOM_LIMIT;

        graph.top++;
        graph.top %= GRAPH_ELEMENT;
        pthread_mutex_unlock(&mutex_graph);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}

void draw_information() {
    int v_current = 0; // current value
    char s[4];         // string to be printed

    pthread_mutex_lock(&mutex_graph);
    if (graph.top)
        v_current = graph.elem[graph.top - 1];
    pthread_mutex_unlock(&mutex_graph);

    acquire_screen();
    sprintf(s, "%i", v_current);
    
    textout_ex(screen, font, "     ", SXT_D, SYT_SCURRENT, 0, 0);
    textout_ex(screen, font, s, SXT_D, SYT_SCURRENT, TEXT_COLOR, 0);

    release_screen();
}

void draw_image(int *last_draw) {

    const unsigned int line_element =
        (unsigned int)(IMAGE_HEIGHT - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT;
    int i, x, y, j;
    int size = GRAPH_ELEMENT * line_element;
    BITMAP *image_bmp = create_bitmap(size, size);
    BITMAP *row_bmp;

    acquire_screen();
    pthread_mutex_lock(&mutex_graph);

    for (*last_draw;
         *last_draw != (graph.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT;
         *last_draw = ++(*last_draw) % GRAPH_ELEMENT) {
        x = IMAGE_X2 + INTERNAL_MARGIN + line_element * (*last_draw);
        y = IMAGE_Y2 + INTERNAL_MARGIN;

        rectfill(screen, x, y, x + line_element - 1, y + line_element - 1,
                 graph.elem[*last_draw]);

        if (*last_draw == GRAPH_ELEMENT - 1) {
            blit(screen, image_bmp, IMAGE_X2 + INTERNAL_MARGIN,
                 IMAGE_Y2 + INTERNAL_MARGIN, 0, 0, size, size);

            blit(image_bmp, screen, 0, 0, IMAGE_X2 + INTERNAL_MARGIN,
                 IMAGE_Y2 + INTERNAL_MARGIN + line_element, size, size);

            row_bmp = create_sub_bitmap(screen, IMAGE_X2 + INTERNAL_MARGIN,
                                        IMAGE_Y2 + INTERNAL_MARGIN, size,
                                        line_element);
            clear_bitmap(row_bmp);
        }
    }

    pthread_mutex_unlock(&mutex_graph);
    release_screen();
}

void *graphic_task() {
    struct timespec t;
    int period = 30;
    int ld_image = 0, ld_graph = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    init_interface();
    draw_background();

    while (1) {
        draw_graphic(&ld_graph);
        draw_information();
        draw_image(&ld_image);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }

    allegro_exit();
}

void get_keycodes(char *scan) {
    int k = readkey();
    *scan = k >> 8;
}

int task_create(Task *t) {
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    param.sched_priority = (*t).priority;
    pthread_attr_setschedparam(&attr, &param);

    return pthread_create(&(*t).id, &attr, (*t).f, NULL);
}

int main() {
    int index;
    Task task_table[] = {
        {-1, simulate_sensor_task, 25},
        {-1, graphic_task, 20},
    };
    const int n_task = sizeof(task_table) / sizeof(Task);

    pthread_mutex_init(&mutex_graph, NULL);
    // pthread_mutex_init(&mutex_cursor, NULL);

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    do {
    } while (!key[KEY_ESC]);

    for (index = 0; index < n_task; index++)
        pthread_cancel(task_table[index].id);

    return 0;
}