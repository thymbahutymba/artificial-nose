#include "neural_network.h"

TF_Graph *graph;              // Graph associated to trained neural network
TF_Status *status;            // Result status of tensorflow execution
TF_Session *session;          // Tensorflow session to calculate the results
TF_SessionOptions *sess_opts; // Session option for the new session
TF_Tensor *out_vals;          // Tensor that contains results of execution
TF_Output input_op, output;   // Input and output layers

/* Free the data previously allocated for tensorflow graph */
void free_buffer(void *data __attribute__((unused)),
                 size_t length __attribute__((unused))) {}

/* Free the data associated to input tensor */
static void deallocator(void *data __attribute__((unused)),
                        size_t length __attribute__((unused)),
                        void *arg __attribute__((unused))) {}

/* Read file containing the graph of the neural network and initialize the
 * TF_Buffer */
TF_Buffer *read_file(const char *file) {
    // Open file that contains graph
    FILE *f = fopen(file, "rb");

    /* Move the position indicator to the end of file for compute the exact file
     * size */
    fseek(f, 0, SEEK_END);
    long int fsize = ftell(f);

    // Move back the position indicator to the begging of file
    fseek(f, 0, SEEK_SET);

    // Read the graph from file and put it in the allocated array
    fread(f_graph, fsize, 1, f);
    fclose(f);

    // TF_Buffer used by tensorflow to refer the graph
    TF_Buffer *buf = TF_NewBuffer();

    buf->data = f_graph; // Pointer where the graph is saved
    buf->length = fsize; // Real dimension of the graph
    buf->data_deallocator = free_buffer;
    return buf;
}

/* Import the graph in the main graph */
void import_graph() {
    // Read graph from file
    TF_Buffer *graph_def = read_file(GRAPH_NAME);

    // Import the graph serialized in graph_def into graph
    TF_ImportGraphDefOptions *opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, opts, status);

    // Check if something is gone wrong
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s", TF_Message(status));
        return;
    }

    // Deallocation of tensorflow object previously allocated
    TF_DeleteImportGraphDefOptions(opts);
    TF_DeleteBuffer(graph_def);
}

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
            /* Gets the 15-bit pixel color of the stretched image in the given
             * location */
            color = _getpixel15(str_img, col, row);

            /* Extracts the red, green and blue component from color and
             * normalize it from 0 to 1 */
            data[row][col][0] = (float)getr15(color) / MAX_CC;
            data[row][col][1] = (float)getg15(color) / MAX_CC;
            data[row][col][2] = (float)getb15(color) / MAX_CC;
        }

    // Destroy the bitmap previously allocated
    destroy_bitmap(str_img);
    release_screen();
}

/* Initialization of input tensor with resized image */
TF_Tensor *tf_init_input() {
    TF_Tensor *input_tensor; // Tensor containing input image

    // Third order tensor that contains the resized bitmap
    float data[FIXED_S][FIXED_S][CHANNELS];

    // Number of bytes for input
    const unsigned int nb_in = ARRAY_SIZE * sizeof(float);

    // Input dimensions
    int64_t in_dims[] = {1, FIXED_S, FIXED_S, CHANNELS};
    int n_in_dims = sizeof(in_dims) / sizeof(int64_t);

    // Resizing and converting image as tensorflow model wants
    resize_and_convert(data);

    // Creation the input tensor used by tensorflow session
    input_tensor = TF_NewTensor(TF_FLOAT, in_dims, n_in_dims, data, nb_in,
                                &deallocator, 0);

    return input_tensor;
}

/* Initialization tensor that contains the results */
void tf_init_output() {
    // Number of bytes for output
    const int nb_out = N_LAB * sizeof(float);

    // Output dimensions
    int64_t out_dims[] = {1, N_LAB};
    int n_out_dims = sizeof(out_dims) / sizeof(int64_t);

    // Tensor that contains the results given by neural network
    out_vals = TF_AllocateTensor(TF_FLOAT, out_dims, n_out_dims, nb_out);
}

/* Calculate the result having as input the current image */
void run_session() {
    // Create input tensor
    TF_Tensor *input_tensor = tf_init_input();

    // Run the graph associated with the session
    TF_SessionRun(session,
                  NULL,                        // Run options
                  &input_op, &input_tensor, 1, // in: tensor, values, number
                  &output, &out_vals, 1,       // out: tensor, values, number
                  NULL, 0,                     // target operation, num targets
                  NULL,                        // metadata
                  status                       // outputs status
    );

    // Check status of operation and set the results
    if (TF_GetCode(status) == TF_OK) {
        pthread_mutex_lock(&mutex_res);
        result = TF_TensorData(out_vals);
        pthread_mutex_unlock(&mutex_res);
    } else
        fprintf(stderr, "%s\n", TF_Message(status));

    // Delete the input tensor not more necessary
    TF_DeleteTensor(input_tensor);
}

/* Clean all stuff allocated by tensorflow before the thread termination */
void tf_exit() {
    // Close and delete tensorflow session
    TF_CloseSession(session, status);
    TF_DeleteSession(session, status);

    TF_DeleteSessionOptions(sess_opts); // Destroy session options
    TF_DeleteStatus(status);   // Delete the object containing the status
    TF_DeleteGraph(graph);     // Delete the object containing the graph
    TF_DeleteTensor(out_vals); // Delete the object containing the results
}

/* Initialization of all stuff required by tensorflow */
void tf_init() {
    // Allocate session option for the session
    sess_opts = TF_NewSessionOptions();

    // Tensorflow object refers neural network
    graph = TF_NewGraph();
    // Tensorflow object that contains the status of the operations
    status = TF_NewStatus();

    // Import graph from file
    import_graph();

    // Initialize the input layer of neural network
    input_op.oper = TF_GraphOperationByName(graph, IN_NAME);
    input_op.index = 0;

    // Initialize the output layer of neural network
    output.oper = TF_GraphOperationByName(graph, OUT_NAME);
    output.index = 0;

    // Create new session associated to the graph
    session = TF_NewSession(graph, sess_opts, status);

    // Initialization of output tensor
    tf_init_output();
}

void *neural_network_task() {
    struct timespec t;  // Time refering the period
    struct timespec dl; // Time refering the deadline

    // Initialization of tensorflow and object needed by it
    tf_init();

    // Set cancel mode as asynchronous
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // Push the routine that is executed after receiving cancellation request
    pthread_cleanup_push(&tf_exit, NULL);

    set_activation(&t, task_table[NN_I].period);
    set_activation(&dl, task_table[NN_I].period);

    while (1) {
        run_session();
        check_deadline(&dl, NN_I);
        wait_for_activation(&t, &dl, task_table[NN_I].period);
    }

    pthread_cleanup_pop(1);
}