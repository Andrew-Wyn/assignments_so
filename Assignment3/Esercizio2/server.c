#include "util.h"

int listenfd, running = 1;

int process_string(char* in, char* out, int n) {
    for (int i=0; i<n; i++) {
        if (isspace(in[i]) || in[i] == '\f' || in[i] == '\r' || in[i] == '\t' || in[i] == '\v') {
            return 0; // false errore
        }

        if (islower(in[i])) {
            out[i] = toupper(in[i]);
        } else if (isupper(in[i])) {
            out[i] = tolower(in[i]);
        } else {
            out[i] = in[i];
        }
    }
    out[n] = '\0';
    return 1; // true tutto ok
}

// routine del thread addetto all'handling di tutti i segnali non gestiti dagli altri
void* master_handler(void* arg) {
    // tale è l'unico thread di tutto l'applicativo server in grado di sentire i segnali definiti dal testo dell'esercizio
    // non potendo gestire i segnali con un sighandler allora, semplicemente faccio si che durante

    sigset_t *set = arg;
    int sig;

    // ciclo sopra sigwait la quale si stoppa finche uno dei segnali settati a 1 nella maschera passata come input
    // non diventa pending (non gestito) in tal momento se il segnale è tra quelli definiti dal testo (controllo ridondante poiche normalmente i segnali non entrano in pending hanno tutti un handler di default)
    for (;;) {
        // The  sigwait() function suspends execution of the calling thread until one of the signals specified in the signal set set becomes pending.
        Sigwait(set, &sig);
        if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP) {
            printf("catched %d\n", sig);
            Shutdown((intptr_t) listenfd, SHUT_RDWR); // faccio terminare le operazioni sul socket
            Close(listenfd);
            Unlink(SOCKNAME);
            running = 0;
            break;
        }
    }
 
    pthread_exit(NULL);
    return NULL;
}

void* server_sub_routine(void* _fd) {

    int fd = (intptr_t) _fd;

    char in_buf[BUFSIZE];
    memset(in_buf, 0, BUFSIZE);
    int n = 0;

    while ((n = readn(fd, in_buf, BUFSIZE)) > 0) { // quando legge 0 significa che il client ha chiuso la connessione
        fflush(stdout);
        // process string 
        char out_buf[strlen(in_buf)];
        memset(out_buf, '\0', strlen(in_buf));
        if (process_string(in_buf, out_buf, strlen(in_buf))) { // effettua la procedura per modificare la stringa
            if (writen(fd, out_buf, BUFSIZE) == -1) { // se ci sono errori sulla write termino il thread
                perror("write");
                break;
            }
        } else {
            if (writen(fd, "errore conversione", 19) == -1) {
                perror("write");
                break;
            }
        } 
    }

    if (n == -1) { // se readn ritorna -1 c'è stato un errore ma comunque chiudo la connessione e termino il thread
        perror("read");
    }

    printf("[SERVER] chiudo la connessione\n");

    Close(fd);

    pthread_exit(NULL);
    return NULL;
}

int main() {

    // creo una nuova maschera con settati a 1 (inibendoli) i segnali dati dal testo facendoli gestire dal thread master_handler
    sigset_t mask;
    sigemptyset(&mask);
    Sigaddset(&mask, SIGINT);
    Sigaddset(&mask, SIGQUIT);
    Sigaddset(&mask, SIGTERM);
    Sigaddset(&mask, SIGHUP);
    Sigaddset(&mask, SIGPIPE); // inibisco sigpipe per evitare il termine del processo quando una write non va a buonfine causa connessione persa
    // aggiungo tale maschera alla maschera del processo, con tale metodo la maschera sara ereditata da tutti i thread generati dal principale
    Pthread_sigmask(SIG_BLOCK, &mask, NULL); // la nuova signal mask diventa l’or di set e della vecchia signal mask (i segnali in set sono aggiunti alla maschera)

    printf("server\n");

    // creo un nuovo socket 
    listenfd = Socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);
    Bind(listenfd, (struct sockaddr*) &address, sizeof(address));
    Listen(listenfd, SOMAXCONN);
    
    // thread master handler
    pthread_t master_handler_th;
    Pthread_create(&master_handler_th, NULL, master_handler, (void*) &mask);

    int fd;

    while (running) {
        fd = accept(listenfd, (struct sockaddr*) NULL, NULL);
        if (fd == -1) continue; // l'errore viene invocato quando effettuo uno shutdown sul descrittore listen fd
        // creare thread connessione
        pthread_t client_thread;
        pthread_attr_t attr_thread;
        // i thread devono essere creati in detatch mode in modo tale che non devo richiamare join per liberarne le risorse
        Pthread_attr_init(&attr_thread);
        Pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED);
        Pthread_create(&client_thread, &attr_thread, server_sub_routine, (void*) (intptr_t) fd);
    }

    pthread_join(master_handler_th, NULL); //

    return 0;
}