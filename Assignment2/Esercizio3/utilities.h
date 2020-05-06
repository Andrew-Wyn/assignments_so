#ifndef UTILITIES_H
#define UTILITIES_H

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define SHRMEM "/minimetro"

typedef struct biglietto {
    int id;
    int fermata_; /* 0: stazione, 1: centro storico */
} biglietto;

typedef struct
{
    /* variabili per la sincronizzazione */
    pthread_mutex_t mtx;
    pthread_cond_t cond_staz;
    pthread_cond_t cond_cc;
    pthread_cond_t cond_piena;
    pthread_cond_t cond_arrivata;
    pthread_cond_t cond_scesi_tutti;
    pthread_cond_t cond_waitfull;

    int ubriachi; /* 0 o 1 */
    int num_passengers;

    int ids[4];

    // numero passeggeri che sono in attesa
    int waitlen;

    // necessario senno posso ritrovarmi in una situazione di stallo
    int sobri_cc;
    int sobri_stazione;

} buffer_condiviso;

// funzioni
void print_passengers(buffer_condiviso* buffer);
void Pthread_mutex_lock(pthread_mutex_t* mtx);
void Pthread_mutex_unlock(pthread_mutex_t* mtx);
void Pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mtx);
void Pthread_cond_signal(pthread_cond_t* cond);
void Pthread_cond_broadcast(pthread_cond_t* cond);
void Pthread_mutex_init(pthread_mutex_t* mtx, pthread_mutexattr_t* mtxattr);
void Pthread_mutexattr_init(pthread_mutexattr_t* mtxattr);
void Pthread_mutexattr_destroy(pthread_mutexattr_t* mattr);
void Pthread_condattr_init(pthread_condattr_t* condattr);
void Pthread_cond_init(pthread_cond_t* cond, pthread_condattr_t* condattr);
void Pthread_create(pthread_t* thread, pthread_attr_t* attr, void *(*__start_routine) (void *), void* args);
void Pthread_condattr_destroy(pthread_condattr_t* condattr);
void Pthread_mutexattr_setpshared(pthread_mutexattr_t* mattr, int _pshared);
void Pthread_condattr_setpshared(pthread_condattr_t* condattr, int _pshared);
int Shm_open(const char *__name, int __oflag, mode_t __mode);
void Ftruncate(int shmfd, int shared_seg_size);
void* Malloc(size_t sizeToAlloc);
#endif