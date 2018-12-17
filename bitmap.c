#include <allegro.h>
#include <stdio.h>

int main() {
    BITMAP *img, *save;
    PALETTE pal;
    unsigned short data[444 * 440];
    int tmp;

    allegro_init();
    set_color_depth(16);
    img = load_bitmap("image_neural_network/aglio/image_0000.bmp", pal);
    save = create_bitmap(444, 440);
    /*
    ssize_t x;
    ssize_t line = 0;
    size_t c;

    for (line = 0; line < img->h; ++line)
        for (x = 0; x < img->w; x++)
                data[line * img->w + x] =
                    ((unsigned short *)img->line[line])[x];

    for (line = 0; line < save->h; ++line)
        for (x = 0; x < save->w; ++x)
                ((unsigned short *)save->line[line])[x] =
                    data[line * save->w + x];
*/
    tmp = _getpixel16(img, 0, 0);
    printf("%i %i %i\n", getr(tmp), getg(tmp), getb(tmp));

    //get_palette(pal);
    //save_bmp("prova.bmp", save, pal);

    //printf("\n");
}