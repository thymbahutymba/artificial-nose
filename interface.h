#ifndef INTERFACE_H
#define INTERFACE_H

#include "ptask.h"
#include "sensor.h"
#include <allegro.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH (1024)
#define SCREEN_HEIGHT (768)
#define EXTERNAL_MARGIN (5)               // space between sections borders
#define INTERNAL_MARGIN (8)               // internal section space
#define BORDER_COLOR (0b1111111111100000) // yellow color for border section
#define MAIN_COLOR (0b1111100000000000)   // light red for title
#define TEXT_COLOR (0b1111111111111111)   // white for text color
#define GRAPH1_COLOR (0b1111100000000000) // red for CO2 graph
#define GRAPH2_COLOR (0b0000011111100000) // green for tVOC gragh
#define BKG_COLOR (0)

/**********************************************************
 * IMAGE
 **********************************************************/

#define IMAGE_WIDTH (460)
#define IMAGE_HEIGHT (460)

#define IMAGE_X1 (SCREEN_WIDTH - EXTERNAL_MARGIN)
#define IMAGE_Y1 (SCREEN_HEIGHT - EXTERNAL_MARGIN)
#define IMAGE_X2 (IMAGE_X1 - IMAGE_WIDTH)
#define IMAGE_Y2 (IMAGE_Y1 - IMAGE_HEIGHT)

/**********************************************************
 * SUBBOX
 **********************************************************/

#define G_SUBBOX (40)

#define SUBBOX_X1 (EXTERNAL_MARGIN)
#define SUBBOX_Y1 (IMAGE_Y2)
#define SUBBOX_X2 (SCREEN_WIDTH - EXTERNAL_MARGIN * 2 - IMAGE_WIDTH)
#define SUBBOX_Y2 (SUBBOX_Y1 + G_SUBBOX)

#define SXT_S (SUBBOX_X1 + INTERNAL_MARGIN) // x position of static text
#define SYT_SCURRENT (IMAGE_Y2 + INTERNAL_MARGIN * 2) // text row for current
#define SXT_CO2 (SXT_S + 180)  // dynamc text alignment for CO2
#define SXT_TVOC (SXT_S + 288) // dynamc text alignment for tVOC

/**********************************************************
 * GRAPH
 **********************************************************/

#define GRAPH_ELEMENT (55)

#define GRAPH_WIDTH (SCREEN_WIDTH - EXTERNAL_MARGIN * 3 - IMAGE_WIDTH)
#define GRAPH_HEIGHT (IMAGE_HEIGHT - G_SUBBOX - EXTERNAL_MARGIN)

// coordinates for each rectangle that delimit sections of screens
#define GRAPH_X1 (EXTERNAL_MARGIN)
#define GRAPH_Y1 (SCREEN_HEIGHT - EXTERNAL_MARGIN)
#define GRAPH_X2 (GRAPH_X1 + GRAPH_WIDTH)
#define GRAPH_Y2 (GRAPH_Y1 - GRAPH_HEIGHT)

/**********************************************************
 * RESULTS
 **********************************************************/

#define RESULT_X1 (EXTERNAL_MARGIN)
#define RESULT_Y1 (EXTERNAL_MARGIN)
#define RESULT_X2 (650)
#define RESULT_Y2 (IMAGE_Y2 - EXTERNAL_MARGIN) // GRAPH_Y2-EXTERNAL_MARGIN

/**********************************************************
 * LEGEND
 **********************************************************/

#define LEGEND_X1 (RESULT_X2 + EXTERNAL_MARGIN)
#define LEGEND_Y1 (EXTERNAL_MARGIN)
#define LEGEND_X2 (SCREEN_WIDTH - EXTERNAL_MARGIN)
#define LEGEND_Y2 (RESULT_Y2)

/* legend text location */
#define LTEXT_X (LEGEND_X1 + INTERNAL_MARGIN)
#define LTEXT_Y (LEGEND_Y1 + INTERNAL_MARGIN)
#define LINE_SPACE (12) // space between each line of text

/**********************************************************
 * DEFINITION AND DECLARATION
 **********************************************************/

typedef struct {
    unsigned int top, first;
    int x_point[GRAPH_ELEMENT];   // CHANGE NAME WITH A MORE USEFUL WORD
    uint16_t co2[GRAPH_ELEMENT];  // array for CO2 data from sensor
    uint16_t tvoc[GRAPH_ELEMENT]; // array for tVOC data from sensor
} Queue;

Queue r_data; // data readed by sensor and printed by graphich task
pthread_mutex_t mutex_data;

void *graphic_task();
void *store_image_task();
void init_interface();

#endif // INTERFACE_H