#ifndef PEPLINLABS_RWLOCK_WRITERS_H
#define PEPLINLABS_RWLOCK_WRITERS_H

#include <pthread.h>

struct rwlock_type {
    pthread_mutex_t mutex;
    pthread_cond_t readers;
    pthread_cond_t writers;
    int readers_counter;
    int reading_block_wait;
    int writing_block_wait;
    bool is_writer_blocked;
};

int rwlock_init(rwlock_type *rwlock) {
    pthread_mutex_init(&rwlock->mutex, nullptr);
    pthread_cond_init(&rwlock->readers, nullptr);
    pthread_cond_init(&rwlock->writers, nullptr);

    rwlock->readers_counter = 0;
    rwlock->reading_block_wait = 0;
    rwlock->writing_block_wait = 0;
    rwlock->is_writer_blocked = false;

    return 0;
}

int rwlock_rdlock(rwlock_type *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);

    while (rwlock->writing_block_wait > 0 || rwlock->is_writer_blocked) {
        rwlock->reading_block_wait++;
        pthread_cond_wait(&rwlock->readers, &rwlock->mutex);
        rwlock->reading_block_wait--;
    }

    rwlock->readers_counter++;
    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}

int rwlock_wrlock(rwlock_type *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);
    rwlock->writing_block_wait++;

    while (rwlock->readers_counter > 0 || rwlock->writing_block_wait > 0) {
        pthread_cond_wait(&rwlock->writers, &rwlock->mutex);
    }

    rwlock->is_writer_blocked = true;
    rwlock->writing_block_wait--;
    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}

int rwlock_unlock(rwlock_type *rwlock) {
    pthread_mutex_lock(&rwlock->mutex);

    if (rwlock->is_writer_blocked) {
        rwlock->is_writer_blocked = false;
        pthread_cond_broadcast(&rwlock->readers);
        pthread_cond_signal(&rwlock->writers);
    } else {
        rwlock->readers_counter--;
        if (rwlock->readers_counter == 0) {
            pthread_cond_signal(&rwlock->writers);
        }
    }

    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}

int rwlock_destroy(rwlock_type *rwlock) {
    pthread_mutex_destroy(&rwlock->mutex);
    pthread_cond_destroy(&rwlock->readers);
    pthread_cond_destroy(&rwlock->writers);

    return 0;
}

#endif
