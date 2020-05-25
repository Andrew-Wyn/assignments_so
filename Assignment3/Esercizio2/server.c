#include "util.h"

// aggiungere gestione segnale SIGPIPE per la scrittura su un socket chiuso

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

void* master_handler(void* arg) {
    // tale è l'unico thread di tutto l'applicativo server in grado di sentire i segnali
    // non potendo gestire i segnali con un sighandler allora, semplicemente faccio si che durante

    sigset_t *set = arg;
    int sig;

    for (;;) {
        sigwait(set, &sig); //
        if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP) {
            printf("catched %d\n", sig);
            shutdown((intptr_t) listenfd, SHUT_RDWR);
            unlink(SOCKNAME);
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
    
    while ((n = read(fd, in_buf, BUFSIZE)) > 0){
        fflush(stdout);
        // process string 
        char out_buf[strlen(in_buf)];
        memset(out_buf, '\0', strlen(in_buf));
        if (process_string(in_buf, out_buf, strlen(in_buf)))
            write(fd, out_buf, BUFSIZE);
        else 
            write(fd, "errore conversione", 19);
    }
    
    printf("[SERVER] chiusa la connessione\n");

    close(fd); //

    pthread_exit(NULL);
    return NULL;
}

int main() {

    // SIGINT, SIGQUIT, SIGTERM, SIGHUP
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        pthread_exit(NULL);
    }

    printf("server\n");

    
    listenfd = socket(AF_UNIX, SOCK_STREAM, 0); //
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);
    bind(listenfd, (struct sockaddr*) &address, sizeof(address)); //
    listen(listenfd, SOMAXCONN);
    
    // gestire master handler
    pthread_t master_handler_th;
    pthread_create(&master_handler_th, NULL, master_handler, (void*) &mask);

    int fd;

    while (running) {
        fd = accept(listenfd, (struct sockaddr*) NULL, NULL); //
        if (fd == -1) continue;
        // creare thread connessione
        pthread_t client_thread;
        pthread_attr_t attr_thread;
        // i thread devono essere creati in detatch mode (void*) (intptr_t) 0
        pthread_attr_init(&attr_thread); //
        pthread_attr_setdetachstate(&attr_thread, PTHREAD_CREATE_DETACHED); // 
        pthread_create(&client_thread, &attr_thread, server_sub_routine, (void*) (intptr_t) fd); //
    }

    pthread_join(master_handler_th, NULL);
    


    return 0;
}