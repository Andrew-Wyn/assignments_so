#include "util.h"

int main() {

    printf("client\n");

    struct sockaddr_un address;
    int fd;
    fd = socket(AF_UNIX, SOCK_STREAM, 0); //
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);
    connect(fd, (struct sockaddr*) &address, sizeof(address)); //

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