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
void import_graph(TF_Graph *graph, TF_Status *status) {
    TF_Buffer *graph_def = read_file(GRAPH_NAME);

    TF_ImportGraphDefOptions *opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, opts, status);
    TF_DeleteImportGraphDefOptions(opts);
    // check status
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));
        return;
    }

    TF_DeleteBuffer(graph_def);
}

static void Deallocator(void *data, size_t length, void *arg) {}

void stretch_and_linear(tfdat_t *data) {
    BITMAP *str_img = create_bitmap(299, 299);
    PALETTE pal;

    acquire_screen();

    stretch_blit(screen, str_img, IMG_XT, IMG_YT, EL_W, ACT_IMG_H, 0, 0,
                 str_img->w, str_img->h);

    ssize_t x;
    ssize_t line;

    // All red all green all blue for each rows
    for (line = 0; line < str_img->h; ++line)
        for (x = 0; x < str_img->w; ++x) {
            int color = _getpixel16(str_img, x, line);
            data[line * str_img->w + x] = (tfdat_t)getr16(color) / (1 << 8);
            data[line * str_img->w + x + 1 * str_img->w * str_img->h] =
                (tfdat_t)getg16(color) / (1 << 8);
            data[line * str_img->w + x + 2 * str_img->w * str_img->h] =
                (tfdat_t)getb16(color) / (1 << 8);
        }

    destroy_bitmap(str_img);
    release_screen();
}

void run_session(TF_Graph *graph, TF_Status *status) {
    tfdat_t data[299 * 299 * CHANNELS];

    // Number of bytes of input
    const unsigned int nb_in = 299 * 299 * CHANNELS * sizeof(tfdat_t);

    // Number of bytes of output
    const int nb_out = N_LAB * sizeof(float);

    // Input dimensions
    int64_t in_dims[] = {1, 299, 299, CHANNELS};
    int n_in_dims = sizeof(in_dims) / sizeof(int64_t);

    // Output dimensions
    int64_t out_dims[] = {1, N_LAB};
    int n_out_dims = sizeof(out_dims) / sizeof(int64_t);

    stretch_and_linear(data);

    TF_Output input_op = {TF_GraphOperationByName(graph, IN_NAME), 0};

    TF_Tensor *input_tensor = TF_NewTensor(TF_FLOAT, in_dims, n_in_dims, data,
                                           nb_in, &Deallocator, 0);

    TF_Output output = {TF_GraphOperationByName(graph, OUT_NAME), 0};

    TF_Tensor *output_values =
        TF_AllocateTensor(TF_FLOAT, out_dims, n_out_dims, nb_out);

    TF_SessionOptions *sess_opts = TF_NewSessionOptions();
    TF_Session *session = TF_NewSession(graph, sess_opts, status);

    TF_SessionRun(session,
                  NULL,                        // Run options
                  &input_op, &input_tensor, 1, // in: tensor, values, number
                  &output, &output_values, 1,  // out: tensor, vlaues, number
                  NULL, 0,                     // target operation, num targets
                  NULL,                        // metadata
                  status                       // outputs status
    );

    if (TF_GetCode(status) == TF_OK) {
        pthread_mutex_lock(&mutex_res);
        result = TF_TensorData(output_values);
        pthread_mutex_unlock(&mutex_res);
    } else
        fprintf(stderr, "%s\n", TF_Message(status));
}

void *neural_network_task(void *period) {
    struct timespec t;

    TF_Graph *graph = TF_NewGraph();
    TF_Status *status = TF_NewStatus();

    import_graph(graph, status);

    set_activation(&t, *(int *)period);

    while (1) {
        run_session(graph, status);
        wait_for_activation(&t, *(int *)period);
    }
}