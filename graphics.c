#include <allegro.h>
#include "graphics.h"

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
    };
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

int main()
{
    init_interface();
    draw_background();

    do
    {
    } while (!key[KEY_ESC]);

    allegro_exit();
    return 0;
}