#include <allegro.h>
#include <stdio.h>

int main() {
    // BITMAP *img, *save;
    PALETTE pal;
    // unsigned short data[444 * 440];
    // int tmp;
    BITMAP *test;

    int white = (1 << 16) - 1;

    allegro_init();
    set_color_depth(16);

    test = create_bitmap(2, 2);

    putpixel(test, 0, 0, white);
    putpixel(test, 0, 1, white);
    //putpixel(test, 0, 2, white);

    putpixel(test, 1, 0, 0);
    putpixel(test, 1, 2, 0);
    //putpixel(test, 1, 3, 0);

    putpixel(test, 2, 0, white);
    putpixel(test, 2, 1, white);
    //putpixel(test, 2, 2, white);

    get_palette(pal);
    save_bmp("test.bmp", test, pal);

    /*img = load_bitmap("image_neural_network/aglio/image_0000.bmp", pal);
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
    /*tmp = _getpixel16(img, 0, 0);
    printf("%i %i %i\n", getr(tmp), getg(tmp), getb(tmp));
    */
    // get_palette(pal);
    // save_bmp("prova.bmp", save, pal);

    // printf("\n");
}