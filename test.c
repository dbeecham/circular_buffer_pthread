#include <stdio.h> /* printf */
#include <assert.h> /* assert */

#define CIRCULAR_T int
#include "circular.h"

void test_can_create_free_circular_buffer(void) {
    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));
    //circular_destroy(&buffer);
}

void test_can_enqueue_item() {
    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));
    assert(0 == circular_enqueue(&buffer, 3));
    circular_destroy(&buffer);
}

void test_can_dequeue_item() {

    int popped = 0;

    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));
    assert(0 == circular_enqueue(&buffer, 3));
    assert(0 == circular_pop(&buffer, &popped));
    assert(3 == popped);
    circular_destroy(&buffer);
}

void test_is_queue() {

    int popped = 0;

    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));
    assert(0 == circular_enqueue(&buffer, 1));
    assert(0 == circular_enqueue(&buffer, 2));
    assert(0 == circular_enqueue(&buffer, 2));
    assert(0 == circular_enqueue(&buffer, 2));
    assert(0 == circular_pop(&buffer, &popped));
}

void* _test_is_thread_safe_t2(void * vbuf) {

    struct circular_s * buffer = vbuf;

    int i = 0;
    int item = 0;
    for (; i < 10; i++) {
        assert(0 == circular_pop(buffer, &item));
        assert(i == item);
    }
    return NULL;
}

void test_is_thread_safe() {

    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));

    pthread_t t2;
    pthread_create(&t2, NULL, _test_is_thread_safe_t2, &buffer);

    int i = 0;
    for (; i < 10; i++) {
        assert(0 == circular_enqueue(&buffer, i));
    }

    (void) pthread_join(t2, NULL);

}

void test_can_timeout_gracefully() {
    struct circular_s buffer = {0};
    assert(0 == circular_init(&buffer));
    assert(-1 == circular_timedpop(&buffer, NULL, (struct timespec){ .tv_nsec = 10000 }));
    circular_destroy(&buffer);
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "TEST: can create and free circular buffer...\n");
    test_can_create_free_circular_buffer();
    
    fprintf(stdout, "TEST: can enqueue items...\n");
    test_can_enqueue_item();

    fprintf(stdout, "TEST: can dequeue items...\n");
    test_can_dequeue_item();

    fprintf(stdout, "TEST: is a queue...\n");
    test_is_queue();

    fprintf(stdout, "TEST: is thread safe...\n");
    test_is_thread_safe();

    fprintf(stdout, "TEST: can time out gracefully...\n");
    test_can_timeout_gracefully();

    return 0;
}
