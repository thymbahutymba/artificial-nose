#ifndef INTERFACE_H
#define INTERFACE_H

#include "keyboard.h"
#include "neural_network.h"
#include "ptask.h"
#include "sensor.h"
#include <allegro.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH (1024)              // width of the window
#define SCREEN_HEIGHT (768)              // height of the window
#define EXTERNAL_MARGIN (5)              // space between sections borders
#define INTERNAL_MARGIN (8)              // internal section space
#define BORDER_COLOR (0b111111111100000) // yellow color for border section
#define MAIN_COLOR (0b111110000000000)   // red for title
#define TEXT_COLOR (0b111111111111111)   // white for text color
#define GRAPH1_COLOR (0b111110000000000) // red for CO2 graph
#define GRAPH2_COLOR (0b000001111100000) // green for tVOC gragh
#define BKG_COLOR (0)                    // black for background
#define LINE_SPACE (16)                  // space between each line of text

#define COLOR_MODE (15) // Sets the pixel color format to be used
#define CHANNELS (3)    // RGB Image

// Number of element that it's sampled by sensor
#define GRAPH_ELEMENT (66)

/*******************************************************************************
 * IMAGE
 ******************************************************************************/

// Size of section that contains image
#define IMAGE_WIDTH (480)
#define IMAGE_HEIGHT (480)

// Coordinates for the drawing the rectangle that delimits this section
#define IMAGE_X1 (SCREEN_WIDTH - EXTERNAL_MARGIN)
#define IMAGE_Y1 (SCREEN_HEIGHT - EXTERNAL_MARGIN)
#define IMAGE_X2 (IMAGE_X1 - IMAGE_WIDTH)
#define IMAGE_Y2 (IMAGE_Y1 - IMAGE_HEIGHT)

#define IMG_XT (IMAGE_X2 + INTERNAL_MARGIN) // x starting point for image
#define IMG_YT (IMAGE_Y2 + INTERNAL_MARGIN) // y starting point for image

// Height and width of the rectangle that represents a single element
#define EL_W (IMAGE_WIDTH - INTERNAL_MARGIN * 2)
#define EL_H ((IMAGE_HEIGHT - INTERNAL_MARGIN * 2) / GRAPH_ELEMENT)
#define ACT_IMG_H ((int)EL_H * GRAPH_ELEMENT) // Actual image height

/*******************************************************************************
 * SUBBOX
 ******************************************************************************/

// Height of sub box where is numerically displayed the values of tVOC and CO2
#define G_SUBBOX (40)

// Coordinates for the drawing the rectangle that delimits sub box section
#define SUBBOX_X1 (EXTERNAL_MARGIN)
#define SUBBOX_Y1 (IMAGE_Y2)
#define SUBBOX_X2 (SCREEN_WIDTH - EXTERNAL_MARGIN * 2 - IMAGE_WIDTH)
#define SUBBOX_Y2 (SUBBOX_Y1 + G_SUBBOX)

#define SXT_S (SUBBOX_X1 + INTERNAL_MARGIN) // x position of static text
#define SYT_SCURRENT (IMAGE_Y2 + 18)        // text row for current
#define SXT_CO2 (SXT_S + 190)       // dynamc text alignment for CO2 value
#define SXT_TVOC (SXT_S + 328)      // dynamc text alignment for tVOC value
#define SXT_CO2_TEXT (SXT_S + 152)  // dynamc text alignment for CO2 text
#define SXT_TVOC_TEXT (SXT_S + 280) // dynamc text alignment for tVOC text

/*******************************************************************************
 * KEYBOARD INPUT
 ******************************************************************************/

#define INPUT_H (40) // Height of section where the keyboard buffer is printed

// Coordinates for printing the rectangle that delimits the keyboard input area
#define INPUT_X1 (EXTERNAL_MARGIN)
#define INPUT_Y1 (EXTERNAL_MARGIN)
#define INPUT_X2 (650)
#define INPUT_Y2 (INPUT_Y1 + INPUT_H)

// Position where the text is printed
#define TEXT_X1 (INPUT_X1 + INTERNAL_MARGIN)
#define TEXT_Y1 (INPUT_Y1 + 18)

/*******************************************************************************
 * GRAPH
 ******************************************************************************/

// Size of section that contains the graphs
#define GRAPH_WIDTH (SCREEN_WIDTH - EXTERNAL_MARGIN * 3 - IMAGE_WIDTH)
#define GRAPH_HEIGHT (IMAGE_HEIGHT - G_SUBBOX - EXTERNAL_MARGIN)

// Coordinates for the drawing the rectangle that delimits graphs section
#define GRAPH_X1 (EXTERNAL_MARGIN)
#define GRAPH_Y1 (SCREEN_HEIGHT - GRAPH_HEIGHT - EXTERNAL_MARGIN)
#define GRAPH_X2 (GRAPH_X1 + GRAPH_WIDTH)
#define GRAPH_Y2 (GRAPH_Y1 + GRAPH_HEIGHT)

#define GRAPH_BASE (GRAPH_Y2 - INTERNAL_MARGIN) // Location of x axis

/*******************************************************************************
 * RESULTS
 ******************************************************************************/

// Coordinates for the drawing the rectangle that delimits result section
#define RESULT_X1 (EXTERNAL_MARGIN)
#define RESULT_Y1 (INPUT_Y2 + EXTERNAL_MARGIN)
#define RESULT_X2 (INPUT_X2)
#define RESULT_Y2 (IMAGE_Y2 - EXTERNAL_MARGIN)

// Position where the results is printed
#define RTEXT_X (RESULT_X1 + INTERNAL_MARGIN)
#define RTEXT_Y (RESULT_Y1 + INTERNAL_MARGIN)

/*******************************************************************************
 * LEGEND
 ******************************************************************************/

// Coordinates for the drawing the rectangle that delimits legend section
#define LEGEND_X1 (RESULT_X2 + EXTERNAL_MARGIN)
#define LEGEND_Y1 (EXTERNAL_MARGIN)
#define LEGEND_X2 (SCREEN_WIDTH - EXTERNAL_MARGIN)
#define LEGEND_Y2 (LEGEND_Y1 + 60)

// Coordinates that represents the alignment of text in the legend area
#define LTEXT_X (LEGEND_X1 + INTERNAL_MARGIN)
#define LTEXT_Y (LEGEND_Y1 + INTERNAL_MARGIN)

/*******************************************************************************
 * DEADLINE MISS AND WCET
 ******************************************************************************/

/* Coordinates for drawing the rectangle that delimits the deadline miss and
 * wcet area */
#define DMW_X1 (LEGEND_X1)
#define DMW_Y1 (LEGEND_Y2 + EXTERNAL_MARGIN)
#define DMW_X2 (LEGEND_X2)
#define DMW_Y2 (RESULT_Y2)

#define DMTEXT_X (DMW_X1 + INTERNAL_MARGIN) // X position for deadline miss
#define WCETEXT_X (DMW_X1 + (DMW_X2 - DMW_X1) / 2) // X position for wcet
#define DMWTEXT_Y (DMW_Y1 + INTERNAL_MARGIN)       // Y start point for each row

/*******************************************************************************
 * DEFINITION AND DECLARATION
 ******************************************************************************/

// Struct to store the values read from the sensor
typedef struct {
    unsigned int top;             // last element of the queue
    int x_point[GRAPH_ELEMENT];   // Coordinates x axis
    uint16_t co2[GRAPH_ELEMENT];  // array for CO2 data from sensor
    uint16_t tvoc[GRAPH_ELEMENT]; // array for tVOC data from sensor
} Queue;

Queue r_data; // data read by sensor and printed by graphich task
pthread_mutex_t mutex_data;

extern Task task_table[]; // Link to task table initialized into main file
extern pthread_mutex_t mutex_tt; // Mutex to protect the task table

void *graphic_task();
void *store_image_task();
void init_interface();

#endif // INTERFACE_H