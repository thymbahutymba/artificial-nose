#include "neural_network.h"

/* Frees the data previously allocated for tensorflow graph */
void free_buffer(void *data, size_t length __attribute__((unused))) {
    free(data);
}

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

/* Import the graph in the main graph */
void import_graph(TF_Graph *graph, TF_Status *status) {
    TF_Buffer *graph_def = read_file(GRAPH_NAME);

    TF_ImportGraphDefOptions *opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, opts, status);

    // check status
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));
        return;
    }

    TF_DeleteImportGraphDefOptions(opts);
    TF_DeleteBuffer(graph_def);
}

static void deallocator(void *data __attribute__((unused)),
                        size_t length __attribute__((unused)),
                        void *arg __attribute__((unused))) {}

/* Resize the image as desired by the tensorflow model and inserts the integer
 * values casted to floating point values ​​into a third-order tensor */
void resize_and_convert(float data[FIXED_S][FIXED_S][CHANNELS]) {
    // Bitmap that contains the stretched image
    BITMAP *str_img = create_bitmap(FIXED_S, FIXED_S);
    ssize_t col; // Index of colums among X axis
    ssize_t row; // Index of lines among Y axis
    int color;   // Pixel color acquired from stretched image

    acquire_screen();

    // Stretch image with fixed size as tensorflow model wants
    stretch_blit(screen, str_img, IMG_XT, IMG_YT, EL_W, ACT_IMG_H, 0, 0,
                 str_img->w, str_img->h);

    /* Scan the bitmap row by row, column by column and get pixel color */
    for (row = 0; row < str_img->h; ++row)
        for (col = 0; col < str_img->w; ++col) {
            /* Gets the 16-bit pixel color of the stretched image in the given
             * location */
            color = _getpixel16(str_img, col, row);

            /* Extracts the red, green and blue component from color and
             * normalize it from 0 to 1 */
            data[row][col][0] = (float)getr16(color) / MAX_CC;
            data[row][col][1] = (float)getg16(color) / MAX_CC;
            data[row][col][2] = (float)getb16(color) / MAX_CC;
        }

    destroy_bitmap(str_img);
    release_screen();
}

/* Inizialization of input tensor with resized image */
TF_Tensor *tf_init_input() {
    // Third order tensor that contains the resized bitmap
    float data[FIXED_S][FIXED_S][CHANNELS];

    // Number of bytes of input
    const unsigned int nb_in = ARRAY_SIZE * sizeof(float);

    // Input dimensions
    int64_t in_dims[] = {1, FIXED_S, FIXED_S, CHANNELS};
    int n_in_dims = sizeof(in_dims) / sizeof(int64_t);

    // Resizing and converting image as tensorflow model wants
    resize_and_convert(data);

    // Tensor that represents the image
    TF_Tensor *input_tensor = TF_NewTensor(TF_FLOAT, in_dims, n_in_dims, data,
                                           nb_in, &deallocator, 0);

    return input_tensor;
}

/* Inizialization tensor that contains the results */
TF_Tensor *tf_init_output() {
    // Number of bytes of output
    const int nb_out = N_LAB * sizeof(float);

    // Output dimensions
    int64_t out_dims[] = {1, N_LAB};
    int n_out_dims = sizeof(out_dims) / sizeof(int64_t);

    // Tensor that contains the results given by neural network
    TF_Tensor *out_vals =
        TF_AllocateTensor(TF_FLOAT, out_dims, n_out_dims, nb_out);

    return out_vals;
}

void run_session(TF_Session *session, TF_Graph *graph, TF_Status *status,
                 TF_Tensor *out_vals) {

    TF_Output input_op = {TF_GraphOperationByName(graph, IN_NAME), 0};
    TF_Tensor *input_tensor = tf_init_input();

    TF_Output output = {TF_GraphOperationByName(graph, OUT_NAME), 0};

    // Run the graph associated with the session
    TF_SessionRun(session,
                  NULL,                        // Run options
                  &input_op, &input_tensor, 1, // in: tensor, values, number
                  &output, &out_vals, 1,       // out: tensor, vlaues, number
                  NULL, 0,                     // target operation, num targets
                  NULL,                        // metadata
                  status                       // outputs status
    );

    if (TF_GetCode(status) == TF_OK) {
        pthread_mutex_lock(&mutex_res);
        result = TF_TensorData(out_vals);
        pthread_mutex_unlock(&mutex_res);
    } else
        fprintf(stderr, "%s\n", TF_Message(status));

    TF_DeleteTensor(input_tensor);
}

/* Clean all stuff allocated by tensorflow before the thread termination */
void tf_exit(void *args) {
    TF_CloseSession(((struct args *)args)->session,
                    ((struct args *)args)->status);
    TF_DeleteSession(((struct args *)args)->session,
                     ((struct args *)args)->status);
    TF_DeleteStatus(((struct args *)args)->status);
    TF_DeleteSessionOptions(((struct args *)args)->sess_opts);
    TF_DeleteGraph(((struct args *)args)->graph);
    TF_DeleteTensor(((struct args *)args)->out_vals);

    // This printf should be remove in the future
    printf("Cleaned\n");
}

/* Inizialization of all stuff required by tensorflow */
void tf_init(TF_Graph **graph, TF_Status **status, TF_Session **session,
             TF_SessionOptions **sess_opts, struct args *arguments,
             TF_Tensor **out_vals) {
    *graph = TF_NewGraph();
    *status = TF_NewStatus();

    // Import graph from file
    import_graph(*graph, *status);

    // New session with associated graph
    *sess_opts = TF_NewSessionOptions();
    *session = TF_NewSession(*graph, *sess_opts, *status);

    // Inizialization of output tensor
    *out_vals = tf_init_output();

    // Inizialization of struct arguments for future deallocation
    arguments->session = *session;
    arguments->status = *status;
    arguments->sess_opts = *sess_opts;
    arguments->graph = *graph;
    arguments->out_vals = *out_vals;
}

void *neural_network_task(void *period) {
    struct timespec t;
    struct args arguments;    // Arguments for deallocation tensorflow stuff
    TF_Graph *graph = NULL;   // Graph associated to session
    TF_Status *status = NULL; // Result status of tensorflow execution
    TF_SessionOptions *sess_opts = NULL; // Tensorflow session options
    TF_Session *sess = NULL;             //
    TF_Tensor *out_vals = NULL; // Tensor that contains results of execution

    tf_init(&graph, &status, &sess, &sess_opts, &arguments, &out_vals);

    // Not needed i guess, but still remain here for a while
    // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // Push the routine that is executed after receiving cancellation request
    pthread_cleanup_push(&tf_exit, &arguments);

    set_activation(&t, *(int *)period);

    while (1) {
        run_session(sess, graph, status, out_vals);
        wait_for_activation(&t, *(int *)period);
    }

    pthread_cleanup_pop(1);
}