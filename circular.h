#ifndef __CIRCULAR_H
#define __CIRCULAR_H

#ifndef CIRCULAR_LEN
# define CIRCULAR_LEN 64
#endif

#ifndef CIRCULAR_T
# define CIRCULAR_T void * 
#endif

#include <time.h> /* struct timespec */
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>


struct circular_s {
    pthread_mutex_t mutex;
    sem_t enqueue_sem;
    sem_t pop_sem;
    int in;
    int out;
    CIRCULAR_T buffer[CIRCULAR_LEN];
};

int circular_init(struct circular_s * buffer) {

    int ret = 0;

    // memset the buffer to 0 - in case user didn't.
    memset(buffer, 0, sizeof(struct circular_s));

    // initialize semaphores
    ret = sem_init(&buffer->enqueue_sem, 0, CIRCULAR_LEN);
    if (0 > ret) {
        return ret;
    }
    ret = sem_init(&buffer->pop_sem, 0, 0);
    if (0 > ret) {
        return ret;
    }

    // initialize mutex
    ret = pthread_mutex_init(&buffer->mutex, NULL);
    if (0 != ret) {
        return ret;
    }

    return 0;
}

int circular_enqueue(struct circular_s * buffer, CIRCULAR_T value) {

    int ret = 0;

    // wait for space to be available
    ret = sem_wait(&buffer->enqueue_sem);
    if (-1 == ret) {
        return ret;
    }

    // wait for mutex lock
    ret = pthread_mutex_lock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    // add the value to buffer
    buffer->buffer[buffer->in] = value;

    // increment in-cursor
    buffer->in = (buffer->in + 1) % CIRCULAR_LEN;

    // unlock mutex.
    ret = pthread_mutex_unlock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    // increment pop buffer - data is available
    ret = sem_post(&buffer->pop_sem);
    if (-1 == ret) {
        return ret;
    }

    // Everything OK.
    return 0;
}

int circular_timedpop(struct circular_s * buffer, CIRCULAR_T * item, struct timespec wait_duration) {
    // wait for data to be available, but only a while.
    int ret = 0;

    /* Get current time */
    struct timespec ts;
    ret = clock_gettime(CLOCK_REALTIME, &ts);
    if (-1 == ret) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    /* Add time */
    ts.tv_sec += wait_duration.tv_sec;
    ts.tv_nsec += wait_duration.tv_nsec;
    
    /* Guard against sporadic interrupts. */
    while (-1 == (ret = sem_timedwait(&buffer->pop_sem, &ts)) && EINTR == errno)
        continue;

    /* Timed out. */
    // errno might be ETIMEDOUT here, which indicates that we timed out.
    if (-1 == ret) {
        return ret;
    }

    // wait for mutex lock
    ret = pthread_mutex_lock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    CIRCULAR_T result = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % CIRCULAR_LEN;

    // unlock mutex
    ret = pthread_mutex_unlock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    // up the enqueue semaphore
    ret = sem_post(&buffer->enqueue_sem);
    if (-1 == ret) {
        return ret;
    }

    *item = result;

    return 0;

}

int circular_pop(struct circular_s * buffer, CIRCULAR_T * item) {

    int ret = 0;

    // wait for data to be available
    ret = sem_wait(&buffer->pop_sem);
    if (-1 == ret) {
        return ret;
    }

    // wait for mutex lock
    ret = pthread_mutex_lock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    CIRCULAR_T result = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % CIRCULAR_LEN;

    // unlock mutex
    ret = pthread_mutex_unlock(&buffer->mutex);
    if (0 != ret) {
        return ret;
    }

    // up the enqueue semaphore
    ret = sem_post(&buffer->enqueue_sem);
    if (-1 == ret) {
        return ret;
    }

    *item = result;
    return 0;
    
}

void circular_destroy(struct circular_s * buffer) {
    sem_destroy(&buffer->pop_sem);
    sem_destroy(&buffer->enqueue_sem);
    pthread_mutex_destroy(&buffer->mutex);
}

#else

// if we reach this point, then this file has been included once before - when
// it was first included, CIRCULAR_T was defined to some value. Don't add an
// ifndef CIRCULAR_T here - if CIRCULAR_T is *not* defined here, then something
// is very wrong, and the program will not work (differing object files will
// have differing definitions of methods and differing layout of the circular_s
// structure)!

int circular_init(struct circular_s * buffer);
int circular_enqueue(struct circular_s * buffer, CIRCULAR_T value);
int circular_timedpop(struct circular_s * buffer, CIRCULAR_T * item, struct timespec wait_duration);
int circular_pop(struct circular_s * buffer, CIRCULAR_T * item);
void circular_destroy(struct circular_s * buffer);

#endif
