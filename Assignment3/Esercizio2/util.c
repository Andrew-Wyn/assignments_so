# include "util.h"

void Pthread_sigmask(int how, const sigset_t* set, sigset_t* oldset) {
    int ret;
    if ((ret = pthread_sigmask(how, set, oldset)) != 0) {
        errno=ret; // non setta errno lo settiamo noi
        perror("pthread_sigmask");
        exit(errno);
    }
}

int Socket(int domain, int type, int protocol) {
    int ret;
    if ((ret = socket(domain, type, protocol)) == -1) {
        int var_errno=errno;
        perror("socket");
        exit(var_errno);
    }
    return ret;
}

void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    int ret;
    if ((ret = bind(sockfd, addr, addrlen)) == -1) {
        int var_errno=errno;
        perror("bind");
        exit(var_errno);
    }
} 

void Listen(int sockfd, int backlog) {
    int ret;
    if ((ret = listen(sockfd, backlog)) == -1) {
        int var_errno=errno;
        perror("listen");
        exit(var_errno);
    }
}

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void *(*start_routine) (void *), void *arg) {
    int ret;
    if ((ret = pthread_create(thread, attr, start_routine, arg)) != 0) {
        errno=ret; // non setta errno
        perror("pthread_create");
        exit(errno);
    }
}

// int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
//     int ret;
//     if ((ret = accept(sockfd, addr, addrlen)) == -1) {
//         int var_errno=errno;
//         perror("accept");
//         exit(var_errno);
//     }
//     return ret;
// }

void Pthread_attr_init(pthread_attr_t *attr) {
    int ret;
    if ((ret = pthread_attr_init(attr)) != 0) {
        errno=ret;
        perror("pthread_attr_init");
        exit(errno);
    }
}

void Pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    int ret;
    if ((ret = pthread_attr_setdetachstate(attr, detachstate)) != 0) {
        errno=ret;
        perror("pthread_attr_setdetachstate");
        exit(errno);
    }
}

void Sigaddset(sigset_t *set, int signum) {
    int ret;
    if ((ret = sigaddset(set, signum)) == -1){
        int var_errno=errno;
        perror("sigaddset");
        exit(var_errno);
    }
}

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    int ret;
    if ((ret = sigaction(signum, act, oldact)) == -1){
        int var_errno=errno;
        perror("sigaction");
        exit(var_errno);
    }
}

void Connect(int fd, struct sockaddr* address, socklen_t addrlen) {
        while (connect(fd, address, addrlen)) {
        if (errno == ENOENT){
            fflush(stdout);
            sleep(1); /* sock non esiste */
        } else
            exit(EXIT_FAILURE);
    }
}

void Close(int fd) {
    int ret;
    if ((ret = close(fd)) == -1) {
        int var_errno=errno;
        perror("close");
        exit(var_errno);
    }
}

void Unlink(const char *pathname) {
    int ret;
    if ((ret = unlink(pathname)) == -1) {
        int var_errno=errno;
        perror("unlink");
        exit(var_errno);
    }
}

void Shutdown(int sockfd, int how) {
    int ret;
    if ((ret = shutdown(sockfd, how)) == -1) {
        int var_errno=errno;
        perror("shotdown");
        exit(var_errno);
    }
}

void Sigwait(sigset_t *set, int *sig) {
    int ret;
    if ((ret = sigwait(set, sig)) != 0) {
        errno=ret;
        perror("sigwait");
    }
}

