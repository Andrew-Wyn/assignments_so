#include "util.h"

const char* ERROR_READ_WRITE = "[CLIENT]: il server ha chiuso la connessione\n";

// aggiungere gestione segnale SIGPIPE per la scrittura su un socket chiuso
static void sig_pipe_handler(int sig) {
    write(STDERR_FILENO, ERROR_READ_WRITE, strlen(ERROR_READ_WRITE)+1);
    //_exit(0);
}

int main() {

    printf("client\n");

    // add handler for sigpipe error during write when server is offline
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_pipe_handler;
    Sigaction(SIGPIPE, &sa, NULL);
 

    // creo un socket
    struct sockaddr_un address;
    int fd;
    fd = Socket(AF_UNIX, SOCK_STREAM, 0);
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

    // connetto al socket
    printf("connettendo...\n");
    Connect(fd, (struct sockaddr*) &address, sizeof(address));
    printf("connesso!!!\n");
    fflush(stdout);

    char out_buf[BUFSIZE];
    int n_read;
    do {
        // leggo dalla tastiera
        printf("> ");
        fflush(stdout);
        memset(out_buf, '\0', BUFSIZE);
        n_read = read(STDIN_FILENO, out_buf, BUFSIZE-1); // se vengono scritte piu di BUFSIZE caratteri allora ciclera inviando in continuazione messaggi al server finche non svuota il buffer
        if (n_read == 0) { // non leggere nulla da tastiera non è corretto
            break;
        }
        if (n_read == -1) {
            perror("read");
            break;
        }

        // controllo se sto leggendo da un buffer che supera BUFSIZE-1 (ultimo valore null) nel caso non è necessario eliminare la new line
        // ma se la lunghezza di quello che sto leggendo dal buffer piu la new line è uguale a BUFSIZE-1 devo togliere comunque newline
        if (n_read < BUFSIZE-1) {
            out_buf[strlen(out_buf)-1] = '\0';
        }
        else {
            if (out_buf[BUFSIZE-2] == '\n')
                out_buf[BUFSIZE-2] = '\0';
        }


        // se leggo quit esco
        if (strncmp(out_buf, "quit", 4) == 0) break;

        // scrivo al server
        if (writen(fd, out_buf, BUFSIZE) == -1) { // quando cade la connessione
            break;
        }

        // attendo la risposta del server
        n_read = readn(fd, out_buf, BUFSIZE);
        if (n_read == 0) { // chiusa connessione
            fprintf(stderr, "%s", ERROR_READ_WRITE);
            break;
        } 
        if (n_read == -1) {
            perror("read");
            break;
        }
        printf(">> %s\n", out_buf);
        fflush(stdout);
    } while (1);

    printf("[CLIENT]: closed\n");
    Close(fd);
    
    return 0;
}