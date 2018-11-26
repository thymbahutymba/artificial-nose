#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include "graphics.h"

#define BOTTOM_LIMIT 100
#define UPPER_LIMIT 200

//TODO: move to graphic.h
struct Point
{
    struct timespec t; // time in which the value v is readed
    unsigned int v;    // value readed by sensor at time t
};

struct Queue
{
    unsigned int top, first;
    struct Point elem[GRAPH_ELEMENT];
    int x_point[GRAPH_ELEMENT]; // CHANGE NAME WITH A MORE USEFUL WORD
};

struct Queue graph;
pthread_mutex_t mutex_graph;

unsigned int c_cursor, p_cursor; //current and previous position of cursor
//pthread_mutex_t mutex_cursor;

struct Task
{
    pthread_t id;
    void *f;
    int priority;
};

void init_interface()
{
    allegro_init();
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    set_color_depth(8);
    install_keyboard();
}

void draw_background()
{
    int i;
    char *legend_text[] = {
        "ESC: Exit from simulation",
        "ENTER: tbd if necessary",
        "LEFT: move the graph cursor to the left",
        "RIGHT: move the graph cursor to the right"};

    const int legend_element = sizeof(legend_text) / sizeof(char *);

    // draw section for graph location
    rect(screen, GRAPH_X1, GRAPH_Y1, GRAPH_X2, GRAPH_Y2, BORDER_COLOR);
    //line(screen, GRAPH_X1, GRAPH_Y2 + G_SUBBOX, GRAPH_X2, GRAPH_Y2 + G_SUBBOX, BORDER_COLOR);
    rect(screen, SUBBOX_X1, SUBBOX_Y1, SUBBOX_X2, SUBBOX_Y2, BORDER_COLOR);

    // draw section for image location
    rect(screen, IMAGE_X1, IMAGE_Y1, IMAGE_X2, IMAGE_Y2, BORDER_COLOR);

    // draw section for result location
    rect(screen, RESULT_X1, RESULT_Y1, RESULT_X2, RESULT_Y2, BORDER_COLOR);

    // draw section for legend location
    rect(screen, LEGEND_X1, LEGEND_Y1, LEGEND_X2, LEGEND_Y2, BORDER_COLOR);
    textout_ex(screen, font, "LEGEND", LTEXT_X, LTEXT_Y, TITLE_COLOR, 0);

    for (i = 0; i < legend_element; i++)
        textout_ex(screen, font, legend_text[i], LTEXT_X, LTEXT_Y + LINE_SPACE * (i + 1), TEXT_COLOR, 0);
}

void draw_graphic()
{
    int i, j;
    const int offset = (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT);
    //int points[8];
    //int x1, x2, y1, y2, x0, y0, x3, y3;
    //int dist;
    //fixed p1tan, p2tan;
    BITMAP *bmp;
    int c_start = GRAPH_X1 + INTERNAL_MARGIN + offset;
    int p_start = GRAPH_X1 + INTERNAL_MARGIN;

    bmp = create_bitmap(offset * GRAPH_ELEMENT, GRAPH_HEIGHT);

    if (graph.first == graph.top + 1)
    {
        /* NEED TO BE FIXED
        acquire_screen();
        blit(screen, bmp, c_start, GRAPH_Y2, 0, 0, offset * GRAPH_ELEMENT, GRAPH_HEIGHT);
        blit(bmp, screen, 0, 0, p_start, GRAPH_Y2, offset * GRAPH_ELEMENT, GRAPH_HEIGHT);
        release_screen();
        */
        for (i = graph.top, j = 0; i != (graph.top + GRAPH_ELEMENT - 2) % GRAPH_ELEMENT; i = ++i % GRAPH_ELEMENT, ++j)
        {
            acquire_screen();
            fastline(screen, graph.x_point[j], GRAPH_Y1 - graph.elem[i].v, graph.x_point[j + 1], GRAPH_Y1 - graph.elem[(i + 1) % GRAPH_ELEMENT].v, BKG_COLOR);
            release_screen();
        };
    }
    for (i = graph.first, j = 0; i != (graph.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT; i = ++i % GRAPH_ELEMENT, ++j)
    {
        acquire_screen();
        fastline(screen, graph.x_point[j], GRAPH_Y1 - graph.elem[i].v, graph.x_point[j + 1], GRAPH_Y1 - graph.elem[(i + 1) % GRAPH_ELEMENT].v, TEXT_COLOR);
        release_screen();
    }
    pthread_mutex_unlock(&mutex_graph);
}

void time_add_ms(struct timespec *t, int ms)
{
    t->tv_sec += ms / 1000;
    t->tv_nsec += (ms % 1000) * 1000000;
    if (t->tv_nsec > 1000000000)
    {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

/* Initialization of the queue for values that will be sampled */
void init_queue()
{
    int i;
    const int offset = (int)((GRAPH_WIDTH - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT); // distance between values along abscissa

    pthread_mutex_lock(&mutex_graph);
    graph.top = graph.first = 0;

    // inizialization of abscissa values
    for (i = 0; i < GRAPH_ELEMENT; ++i)
        graph.x_point[i] = GRAPH_X1 + INTERNAL_MARGIN + i * offset;

    pthread_mutex_unlock(&mutex_graph);
}

void *simulate_sensor_task()
{
    struct timespec t;
    int period = 150;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    init_queue();

    while (1)
    {
        pthread_mutex_lock(&mutex_graph);
        clock_gettime(CLOCK_MONOTONIC, &graph.elem[graph.top].t);
        graph.elem[graph.top].v = rand() % (UPPER_LIMIT - BOTTOM_LIMIT + 1) + BOTTOM_LIMIT;

        graph.top++;
        graph.top %= GRAPH_ELEMENT;
        if (graph.top == graph.first)
        {
            graph.first++;
            graph.first %= GRAPH_ELEMENT;
        }
        pthread_mutex_unlock(&mutex_graph);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}

// TODO: review this function due to restore all point in graph
/* Remove old cursor from graph and print the new one */
void draw_cursor()
{
    unsigned int xn, yn; // new coordinates of cursor
    unsigned int xo, yo; // previous coordinates of cursor

    pthread_mutex_lock(&mutex_graph);
    // remove previous cursor
    xo = graph.x_point[p_cursor];
    yo = GRAPH_Y1 - graph.elem[p_cursor].v;

    hline(screen, xo - CURSOR_SIZE / 2, yo, xo + CURSOR_SIZE / 2, BKG_COLOR);
    vline(screen, xo, yo - CURSOR_SIZE / 2, yo + CURSOR_SIZE / 2, BKG_COLOR);

    // print new cursor
    xn = graph.x_point[c_cursor];
    yn = GRAPH_Y1 - graph.elem[c_cursor].v;

    hline(screen, xn - CURSOR_SIZE / 2, yn, xn + CURSOR_SIZE / 2, CURSOR_COLOR);
    vline(screen, xn, yn - CURSOR_SIZE / 2, yn + CURSOR_SIZE / 2, CURSOR_COLOR);
    pthread_mutex_unlock(&mutex_graph);
}

void *graphic_task()
{
    struct timespec t;
    int period = 30;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    init_interface();
    draw_background();

    while (1)
    {
        draw_graphic();
        draw_cursor();

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }

    allegro_exit();
}

void get_keycodes(char *scan)
{
    int k = readkey();
    *scan = k >> 8;
}

void *keyboard_task()
{
    struct timespec t;
    int period = 25;
    char scan;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    while (1)
    {
        get_keycodes(&scan);

        // update previous position of cursor
        p_cursor = c_cursor;

        switch (scan)
        {
        case KEY_LEFT:
            pthread_mutex_lock(&mutex_graph);
            if (c_cursor != graph.first)
                c_cursor -= 1;
            pthread_mutex_unlock(&mutex_graph);
            break;
        case KEY_RIGHT:
            pthread_mutex_lock(&mutex_graph);
            if (c_cursor != graph.top)
                c_cursor += 1;
            pthread_mutex_unlock(&mutex_graph);
            break;
        default:
            break;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}

int task_create(struct Task *t)
{
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    param.sched_priority = (*t).priority;
    pthread_attr_setschedparam(&attr, &param);

    return pthread_create(&(*t).id, &attr, (*t).f, NULL);
}

int main()
{
    int index;
    struct Task task_table[] = {
        {-1, simulate_sensor_task, 25},
        {-1, graphic_task, 20},
        {-1, keyboard_task, 20},
    };
    const int n_task = sizeof(task_table) / sizeof(struct Task);

    pthread_mutex_init(&mutex_graph, NULL);
    //pthread_mutex_init(&mutex_cursor, NULL);

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    do
    {
    } while (!key[KEY_ESC]);

    for (index = 0; index < n_task; index++)
        pthread_cancel(task_table[index].id);

    return 0;
}