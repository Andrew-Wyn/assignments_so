#include "util.h"

// aggiungere gestione segnale SIGPIPE per la scrittura su un socket chiuso
static void sig_pipe_handler(int sig) {
    write(STDERR_FILENO, "[CLIENT] errore\n", 17);
    _exit(0);
}

int main() {

    printf("client\n");

    // add handler for sigpipe error during write when server is offline
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_pipe_handler;
    sigset_t handler_mask;
    sigaddset(&handler_mask, SIGPIPE);
    sa.sa_mask  = handler_mask;
    if (sigaction(SIGPIPE, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    struct sockaddr_un address;
    int fd;
    fd = socket(AF_UNIX, SOCK_STREAM, 0); //
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

    printf("connettendo...\n");
    while (connect(fd, (struct sockaddr*) &address, sizeof(address)) == -1) { //
        if (errno == ENOENT){
            fflush(stdout);
            sleep(1); /* sock non esiste */
        } else
            exit(EXIT_FAILURE);
    }

    printf("connesso!!!\n");
    fflush(stdout);

    char out_buf[BUFSIZE];
    do {
        printf("> ");
        fflush(stdout);
        memset(out_buf, '\0', BUFSIZE);
        read(STDIN_FILENO, out_buf, BUFSIZE); //
        out_buf[strlen(out_buf)-1] = '\0';

        if (strncmp(out_buf, "quit", 4) == 0) break;

        write(fd, out_buf, BUFSIZE); //
        read(fd, out_buf, BUFSIZE); //
        printf(">> %s\n", out_buf);
        fflush(stdout);
    } while (1);

    printf("[CLIENT]: closed\n");
    close(fd); //
    
    return 0;
}