# Circular Buffer (using pthreads)

To use this library, include something like this in your `main.c` (or wherever):

    #define CIRCULAR_LEN 128
    #define CIRCULAR_T void * 
    #include "circular.h"

which will give you a 128 element circular buffer, where each element is a void
pointer. You can set `CIRCULAR_T` to some arbitrary type of your choosing, such
as

    #include "your_library.h"
    struct your_structure {};

    #define CIRCULAR_LEN 1024
    #define CIRCULAR_T struct your_structure *
    #include "circular.h"

The API consists of the following methods:

    int circular_init(struct circular_s * buffer);
    int circular_enqueue(struct circular_s * buffer, CIRCULAR_T value);
    int circular_pop(struct circular_s * buffer, CIRCULAR_T * item);
    int circular_timedpop(struct circular_s * buffer, CIRCULAR_T * item, struct timespec wait_duration);
    void circular_destroy(struct circular_s * buffer);

Example usage: