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

#define STAZIONE 0
#define CENTRO_STORICO 1

// struttura dati aggiuntiva che rappresenta i dati relativi ai passeggeri
typedef struct biglietto {
    int id;
    int fermata_; /* 0: stazione, 1: centro storico */
} biglietto;

typedef struct
{
    // variabili per sincronizzazione
    pthread_mutex_t mtx; // mutex comune a tutte le conditional variables
    pthread_cond_t cond_staz; // CV sulla quale aspetteranno i passeggeri in attesa alla stazione
    pthread_cond_t cond_cc; // CV sulla quale aspetteranno i passegeri in attesa al centro storico
    pthread_cond_t cond_piena; // CV sulla quale aspettera la cabina in attesa della salita di un numero sufficiente di passeggeri per permettere la partenza
    pthread_cond_t cond_arrivata; // CV sulla quale aspetteranno i passeggeri entrati nella cabina in attesa del suo arrivo alla fermata di destinazione
    pthread_cond_t cond_scesi_tutti; // CV sulla quale aspettera la cabina in attesa che tutti i passeggeri siano scesi
    pthread_cond_t cond_waitfull; // CV sulla qule aspettera la cabina necessaria solo durante la prima partenza poiche durante la prima signal sulla stazione potrebbero non ancora essersi messi in attesa i passeggeri sufficienti per far funzionare il meccanismo

    int ubriachi; // flag settato dai sobri e indicano ai passeggeri ubriachi che non possono occupare la cabina e devono lasciare il posto
    int num_passengers; // indica il numero di passeggeri all'interno della cabina

    int fermata_attuale; // indica la fermata attuale nella quale si trova la cabina

    int ids[2]; // indica gli id dei passeggeri a bordo

    // necessario senno posso ritrovarmi in una situazione di stallo
    int sobri_cc; // indica il numero dei sobri in attesa alla fermata del centro storico
    int sobri_stazione; // indica il numero dei sobri in attesa alla fermata della stazione

    int ubriachi_cc; // indica il numero degli ubriachi in attesa alla fermata del centro storico
    int ubriachi_stazione; // indica il numero dei sobri in attesa alla fermata della stazione

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