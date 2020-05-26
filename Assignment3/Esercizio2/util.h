/*

Realizzare in C un programma client ed un programma server. 
I due processi intergiscono utilizzando socket AF_UNIX.

SERVER:
- gestisce più connessioni alla volta tramite thread
-- per ogni connessione lancia un nuovo thread che gestirà la connessione con il client
- il messaggio di risposta è una stringa uguale al messaggio originario ma ogni carattere ha invertito il "letter case", i.e., caratteri maiuscoli diventano minuscoli e viceversa
-- Esempio: stringa ricevuta "cIAo" -> stringa restituita "CiaO"
-- Potete evitare il controllo di caratteri speciali (non sara' considerato errore)
-- Il server esegue il controllo se la stringa contiene caratteri "white-space" (' ', \f, \t, \r, \t, \v). In caso ne vengano trovati NON esegue la conversione e restituisce un errore al client
-- SUGGERIMENTO: man 3 islower/isupper; man 3 tolower/toupper, man 3 isspace

CLIENT:
- apre una connessione verso il server ed invia una stringa
- attende il risultato prima di inviare una nuova stringa
- termina quando riceve in input (dall'utente) stringa "quit"

Implementare una gestione dei segnali nel server a seguito della ricezione di uno dei seguenti segnali:
- SIGINT, SIGQUIT, SIGTERM, SIGHUP
- Nel caso di ricezione di uno di questi segnali il server deve terminare lasciando lo stato consistente
- Utilizzare un thread dedicato la gestione dei segnali
-- NON si possono utilizzare signal-handlers (solo eventualmente per SIGPIPE se ritenuto opportuno)
- SUGGERIMENTO: man 2 shutdown per la chiusura del socket AF_UNIX
- SUGGERIMENTO: man 3 pthread_attr_setdetachstate


La soluzione proposta deve contenere anche un makefile che deve generare due eseguibili: uno per il server e uno per il client.
Il codice sorgente del cliente e del server deve essere scritto in due file separati.

*/

#if !defined(UTIL_H)
#define UTIL_H

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#define SOCKNAME "./comunication_socket"
#define BUFSIZE 256

void Pthread_sigmask(int how, const sigset_t* set, sigset_t* oldset);
int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void *(*start_routine) (void *), void *arg);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void Pthread_attr_init(pthread_attr_t *attr);
void Pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
void Sigaddset(sigset_t *set, int signum);
void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
void Connect(int fd, struct sockaddr* address, socklen_t addrlen);
void Close(int fd);
void Unlink(const char *pathname);
void Shutdown(int sockfd, int how);
void Sigwait(sigset_t *set, int *sig);

// nella gestione del sc write quando la connessione viene interrotta, lato client viene gestita 
// tramite handler sul segnale sigpipe, mentre lato server inibiamo il segnale direttamente 
// ma appena ci si rimettera in ascolto la read ritornera 0 e capiremo che la connessione è stata interrotta
// terminando il thread
static inline int writen(long fd, void *buf, size_t size) // gestisce interruzioni lato kernels
{
    size_t left = size;
    int r;
    char *bufptr = (char *)buf;
    while (left > 0)
    {
        if ((r = write((int)fd, bufptr, left)) == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            return 0;
        left -= r;
        bufptr += r;
    }
    return 1;
}

// nella gestione della read durante l'interruzione della connessione ritornera 0 
// e da cio capiamo che la connessione si è interrotta e lato client terminiamo il processo
static inline int readn(long fd, void *buf, size_t size)
{
    size_t left = size;
    int r;
    char *bufptr = (char *)buf;
    while (left > 0)
    {
        if ((r = read((int)fd, bufptr, left)) == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            return 0; // gestione chiusura socket
        left -= r;
        bufptr += r;
    }
    return size;
}

#endif