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

TF_Code run_session(TF_Graph *graph, TF_Status *status, data_t *data) {
    float *result;

    // Number of bytes of input
    const unsigned int nb_in = ARRAY_SIZE * sizeof(data_t);

    // Number of bytes of output 
    /********** CHANGE THIS 3 WITH THE NUMBER OF LABELS READED FROM FILE ******/
    const int nb_out = 3 * sizeof(float);

    // Input dimensions
    int64_t in_dims[] = {1, EL_W, ACT_IMG_H, CHANNELS};
    int n_in_dims = sizeof(in_dims) / sizeof(int64_t);

    // Output dimensions
    /********** CHANGE THIS 3 WITH THE NUMBER OF LABELS READED FROM FILE ******/
    int64_t out_dims[] = {1, 3};
    int n_out_dims = sizeof(out_dims) / sizeof(int64_t);

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

    result = TF_TensorData(output_values);
    printf("%f %f %f\n", result[0], result[1], result[2]);

    printf("%s\n", TF_Message(status));
    return TF_GetCode(status);
}

void image_linearization(data_t *data) {
    BITMAP *image;
    acquire_screen();
    image = create_sub_bitmap(screen, IMG_XT, IMG_YT, EL_W, ACT_IMG_H);

    ssize_t x;
    ssize_t line;

    for (line = 0; line < image->h; ++line)
        for (x = 0; x < image->w; ++x) {
            data[line * image->h + x] = ((data_t *)image->line[line])[x];
        }

    destroy_bitmap(image);
    release_screen();
}

void *neural_network_task(void *period) {
    struct timespec t;
    data_t data[EL_W * ACT_IMG_H * 3];

    TF_Graph *graph = TF_NewGraph();
    TF_Status *status = TF_NewStatus();

    import_graph(graph, status);

    set_activation(&t, *(int *)period);

    while (1) {
        image_linearization(data);

        printf("%i %s\n", run_session(graph, status, data));
        wait_for_activation(&t, *(int *)period);
    }
}