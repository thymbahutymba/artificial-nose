#ifndef GRAPHICS_H
#define GRAPHICS_H

#define SCREEN_WIDTH (1024)
#define SCREEN_HEIGHT (768)
#define EXTERNAL_MARGIN (5) // space between sections borders
#define INTERNAL_MARGIN (8) // internal section space
#define BORDER_COLOR (14)   // yellow color for border section
#define TITLE_COLOR (12)    // light red for title
#define TEXT_COLOR (15)     // white for text color
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

#define G_SUBBOX (60)

#define SUBBOX_X1 (EXTERNAL_MARGIN)
#define SUBBOX_Y1 (IMAGE_Y2)
#define SUBBOX_X2 (SCREEN_WIDTH - EXTERNAL_MARGIN * 2 - IMAGE_WIDTH)
#define SUBBOX_Y2 (SUBBOX_Y1 + G_SUBBOX)

#define SXT_S (SUBBOX_X1 + INTERNAL_MARGIN) // x position of static text
#define SYT_SCURSOR (IMAGE_Y2 + INTERNAL_MARGIN * 2)  // text row for cursor
#define SYT_SCURRENT (IMAGE_Y2 + INTERNAL_MARGIN * 4) // text row for current
#define SXT_D (SXT_S + 120)                            // dynamc text alignment

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
 * CURSOR
 **********************************************************/

#define CURSOR_SIZE (12)
#define CURSOR_COLOR (10)

void init_interface();
void draw_background();

#endif // GRAPHICS_H