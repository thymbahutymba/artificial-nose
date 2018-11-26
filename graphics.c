#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include <string.h>
#include "graphics.h"

#define BOTTOM_LIMIT 100
#define UPPER_LIMIT 200

//TODO: move to graphic.h
struct Point
{
    struct timespec t; // time in which the value v is readed
    unsigned int v;    // value readed by sensor at time t
};

struct ValueQueue
{
    unsigned int top, first;
    struct Point elem[GRAPH_ELEMENT];
};

struct ValueQueue graph;
pthread_mutex_t mutex;

int cursor;
pthread_mutex_t mutex_cursor;

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

//void do_circlefill(BITMAP *bmp, int x, int y, int d){
//    circlefill(bmp, x, y, (int) (d/4), TEXT_COLOR);
//}

void draw_graphic()
{
    int i, j;
    const int d = (int)(GRAPH_WIDTH / GRAPH_ELEMENT);
    int point[8];

    pthread_mutex_lock(&mutex);
    for (i = graph.first; i != (graph.top + GRAPH_ELEMENT - 1) % GRAPH_ELEMENT; i = ++i % GRAPH_ELEMENT)
    {
        /*for(j = 0; j < 8; j++){
            if(!j%2)
                point[j] = GRAPH_X1 + ((i + j / 2 - graph.first) % GRAPH_ELEMENT) * d;
            else
                point[j] = GRAPH_Y1 - graph.elem[(i + j / 2) % GRAPH_ELEMENT].v;
        }
        spline(screen, point, TEXT_COLOR);
        */
        //_putpixel(screen, GRAPH_X1 + (i - graph.first) * d,  GRAPH_Y1 - graph.elem[i].v, TEXT_COLOR);
        //do_line(screen, GRAPH_X1 + (i - graph.first) * d,  GRAPH_Y1 - graph.elem[i].v, \
        //GRAPH_X1 + (i + 1 - graph.first) * d, GRAPH_Y1 - graph.elem[(i + 1) % GRAPH_ELEMENT].v, d, do_circlefill);

        fastline(screen, GRAPH_X1 + (i - graph.first) * d, GRAPH_Y1 - graph.elem[i].v, \
        GRAPH_X1 + (i + 1 - graph.first) * d, GRAPH_Y1 - graph.elem[(i + 1) % GRAPH_ELEMENT].v, TEXT_COLOR);   
    }
    pthread_mutex_unlock(&mutex);
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

void *simulate_sensor_task()
{
    struct timespec t;
    int period = 100;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    while (1)
    {
        pthread_mutex_lock(&mutex);
        clock_gettime(CLOCK_MONOTONIC, &graph.elem[graph.top].t);
        graph.elem[graph.top].v = rand() % (UPPER_LIMIT - BOTTOM_LIMIT + 1) + BOTTOM_LIMIT;

        graph.top++;
        graph.top %= GRAPH_ELEMENT;
        if (graph.top == graph.first)
        {
            graph.first++;
            graph.first %= GRAPH_ELEMENT;
        }
        pthread_mutex_unlock(&mutex);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}

void draw_cursor(){
    int x, y;
    const int d = (int)(GRAPH_WIDTH / GRAPH_ELEMENT);

    pthread_mutex_lock(&mutex_cursor);
    x = GRAPH_X1 + cursor * d;
    y = GRAPH_Y1 - graph.elem[cursor].v;

    hline(screen, x - 10, y, x + 10, TEXT_COLOR);
    vline(screen, x, y - 10, y + 10, TEXT_COLOR);
    pthread_mutex_unlock(&mutex_cursor);
}

void *graphic_task()
{
    struct timespec t;
    int period = 25;

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
        switch (scan)
        {
        case KEY_LEFT:
            pthread_mutex_lock(&mutex_cursor);
            cursor = (cursor - 1) % GRAPH_ELEMENT;
            pthread_mutex_unlock(&mutex_cursor);
            break;
        case KEY_RIGHT:
            pthread_mutex_lock(&mutex_cursor);
            cursor = (cursor + 1) % GRAPH_ELEMENT;
            pthread_mutex_unlock(&mutex_cursor);
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

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_cursor, NULL);

    for (index = 0; index < n_task; index++)
        task_create(&task_table[index]);

    do
    {
    } while (!key[KEY_ESC]);

    for (index = 0; index < n_task; index++)
        pthread_cancel(task_table[index].id);

    return 0;
}