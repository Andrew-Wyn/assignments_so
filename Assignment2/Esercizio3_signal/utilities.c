
#include "utilities.h"

void print_passengers(buffer_condiviso* buffer) {
    printf("composizione cabina: %d passegeri\n", buffer->num_passengers);
    if (buffer->num_passengers == 1) {
        printf("passeggero: %d, UBRIACO\n", buffer->ids[0]);
    } else {
        printf("passeggero: %d, SOBRIO\n", buffer->ids[0]);
        printf("passeggero: %d, SOBRIO\n", buffer->ids[1]);
    }
    fflush(stdout);
}

void Pthread_mutex_lock(pthread_mutex_t* mtx) {
    int err;
    if ((err = pthread_mutex_lock(mtx)) != 0) {
        errno=err;
        perror("pthread_mutex_lock");
        pthread_exit(&errno);
    }
}

void Pthread_mutex_unlock(pthread_mutex_t* mtx) {
    int err;
    if ((err = pthread_mutex_unlock(mtx)) != 0) {
        errno=err;
        perror("pthread_mutex_unlock");
        pthread_exit(&errno);
    }
}

void Pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mtx) {
    int err;
    if ((err = pthread_cond_wait(cond, mtx)) != 0) {
        errno=err;
        perror("pthread_cond_wait");
        pthread_exit(&errno);
    }
}  

void Pthread_cond_signal(pthread_cond_t* cond) {
    int err;
    if ((err = pthread_cond_signal(cond)) != 0) {
        errno=err;
        perror("pthread_cond_signal");
        pthread_exit(&errno);
    }
}

void Pthread_cond_broadcast(pthread_cond_t* cond) {
    int err;
    if ((err = pthread_cond_broadcast(cond)) != 0) {
        errno=err;
        perror("pthread");
        pthread_exit(&errno);
    }
}

void Pthread_mutex_init(pthread_mutex_t* mtx, pthread_mutexattr_t* mtxattr) {
    int err;
    if ((err = pthread_mutex_init(mtx, mtxattr)) != 0) {
        errno=err;
        perror("pthread_mutex_init");
        pthread_exit(&errno);
    } 
}

void Pthread_mutexattr_init(pthread_mutexattr_t* mtxattr) {
    int err;
    if ((err = pthread_mutexattr_init(mtxattr)) != 0) {
        errno=err;
        perror("pthread_mutexattr_init");
        pthread_exit(&errno);
    }
}

void Pthread_condattr_init(pthread_condattr_t* condattr) {
    int err;
    if ((err = pthread_condattr_init(condattr)) != 0) {
        errno=err;
        perror("pthread_condattr_init");
        pthread_exit(&errno);
    }
}

void Pthread_cond_init(pthread_cond_t* cond, pthread_condattr_t* condattr) {
    int err;
    if ((err = pthread_cond_init(cond, condattr)) != 0) {
        errno=err;
        perror("pthread_cond_init");
        pthread_exit(&errno);
    }
}

void Pthread_create(pthread_t* thread, pthread_attr_t* attr, void *(*__start_routine) (void *), void* args) {
    int err;
    if ((err = pthread_create(thread, attr, __start_routine, args)) != 0) {
        errno=err;
        perror("pthread_create");
        pthread_exit(&errno);
    }
}

void Pthread_condattr_destroy(pthread_condattr_t* condattr) {
    int err;
    if ((err = pthread_condattr_destroy(condattr)) != 0) {
        errno=err;
        perror("pthread_condattr_destroy");
        pthread_exit(&errno);
    }
}

void Pthread_mutexattr_destroy(pthread_mutexattr_t* mattr) {
    int err;
    if ((err = pthread_mutexattr_destroy(mattr)) != 0) {
        errno=err;
        perror("pthread_condattr_destroy");
        pthread_exit(&errno);
    }
}

void Pthread_mutexattr_setpshared(pthread_mutexattr_t* mattr, int _pshared) {
    int err;
    if ((err = pthread_mutexattr_setpshared(mattr, PTHREAD_PROCESS_SHARED)) != 0) {
        errno=err;
        perror("pthread_condattr_destroy");
        pthread_exit(&errno);
    }
}

void Pthread_condattr_setpshared(pthread_condattr_t* condattr, int _pshared) {
    int err;
    if ((err = pthread_condattr_setpshared(condattr, PTHREAD_PROCESS_SHARED)) != 0) {
        errno=err;
        perror("pthread_codnattr_setpshared");
        pthread_exit(&errno);
    }
}

int Shm_open(const char *__name, int __oflag, mode_t __mode) {
    int shmfd = shm_open(__name, __oflag, __mode);
    if (shmfd < 0) {
        int var_errno=errno;
        perror("shm_open");
        exit(var_errno);
    }
    return shmfd;
}

void Ftruncate(int shmfd, int shared_seg_size) {
    if (ftruncate(shmfd, shared_seg_size) != 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
}

void* Malloc(size_t sizeToAlloc) {
    void* NEW;
    int var_errno;
    if ((NEW = malloc(sizeToAlloc)) == NULL) {
        var_errno=errno;
        perror("malloc");
        exit(var_errno);
    }
    return NEW;
}