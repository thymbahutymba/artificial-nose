#include "neural_network.h"

void free_buffer(void *data, size_t length) { free(data); }

/* Read file containing the graph of the neural network and initialize the
 * TF_Buffer
 */
TF_Buffer *read_file(const char *file) {
    FILE *f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *data = malloc(fsize);
    fread(data, fsize, 1, f);
    fclose(f);

    TF_Buffer *buf = TF_NewBuffer();
    buf->data = data;
    buf->length = fsize;
    buf->data_deallocator = free_buffer;
    return buf;
}

// import the graph in the main graph
void import_graph(TF_Graph *graph) {
    TF_Buffer *graph_def = read_file("output_graph.pb");

    TF_Status *status = TF_NewStatus();
    TF_ImportGraphDefOptions *opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, opts, status);
    TF_DeleteImportGraphDefOptions(opts);
    // check status
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));
        return;
    }

    fprintf(stdout, "Successfully imported graph");
    TF_DeleteStatus(status);
    TF_DeleteBuffer(graph_def);
}

void *neural_network_task(void *period) {
    struct timespec t;
    TF_Graph *graph = TF_NewGraph();
    unsigned int data[EL_W * ACT_IMG_H];
    ssize_t x;
    ssize_t line;
    BITMAP *buf = create_bitmap(EL_W, ACT_IMG_H);

    set_activation(&t, *(int *)period);

    import_graph(graph);

    while (1) {
        acquire_bitmap(image);

        // For each line in bitmap
        for (line = 0; line < buf->h; ++line)
            for (x = 0; x < buf->w; ++x)
                data[line * image->h + x] =
                    ((unsigned int *)image->line[line])[x];

        /*for (line = 0; line < buf->h; ++line)
            for (x = 0; x < buf->w; ++x)
                ((unsigned int *)buf->line[line])[x] =
                    data[line * image->h + x];
        */
        release_bitmap(image);

        // use neural network

        wait_for_activation(&t, *(int *)period);
    }
}
