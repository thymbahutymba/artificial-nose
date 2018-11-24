#include <allegro.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sched.h>
#include <time.h>
#include "graphics.h"

#define BOTTOM_LIMIT 0
#define UPPER_LIMIT 255

//TODO: move to graphic.h
struct Point
{
    struct timespec t; // time in which the value v is readed
    unsigned int v;    // value readed by sensor at time t
};

struct ValueQueue
{
    int top;
    int last;
    struct Point elem[GRAPH_ELEMENT];
};

struct ValueQueue graph;
pthread_mutex_t mutex;

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
    int period = 50;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);
    
    while (1)
    {
        printf("%i %i", graph.last, graph.top);

        pthread_mutex_lock(&mutex);

        clock_gettime(CLOCK_MONOTONIC, &graph.elem[graph.top].t);
        graph.elem[graph.top].v = rand() % (UPPER_LIMIT + 1);

        graph.top++;
        graph.top %= GRAPH_ELEMENT;
        if (graph.top == graph.last){
            graph.last++;
            graph.last %= GRAPH_ELEMENT;
        }
        pthread_mutex_unlock(&mutex);

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    }
}

int main()
{
    pthread_attr_t attr;
    pthread_t sensor_id;
    graph.top = graph.last = 0;
    unsigned int index;
    char *s;

    struct timespec t;
    int period = 50;

    clock_gettime(CLOCK_MONOTONIC, &t);
    time_add_ms(&t, period);

    pthread_mutex_init(&mutex, NULL);

    init_interface();
    draw_background();

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    pthread_create(&sensor_id, &attr, simulate_sensor_task, NULL);

    do
    {

        pthread_mutex_lock(&mutex);
        if (graph.top != graph.last)
            for(index = graph.last; index < graph.top; index = index++ % GRAPH_ELEMENT){
                sprintf(s, "%li", graph.elem[index].t.tv_sec);
                textout_ex(screen, font, s, RESULT_X1+INTERNAL_MARGIN, RESULT_Y1+INTERNAL_MARGIN, TEXT_COLOR, 0);
            }

        pthread_mutex_unlock(&mutex);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
        time_add_ms(&t, period);
    
    } while (!key[KEY_ESC]);

    allegro_exit();
    return 0;
}