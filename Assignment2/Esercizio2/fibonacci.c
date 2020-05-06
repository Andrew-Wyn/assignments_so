// Scrivere un programma che calcoli ricorsivamente l'X-esimo numero di Fibonacci utilizzando per ogni chiamata un processo distinto.
// - Il valore X deve essere passato come argomento
// -- tale valore deve essere compreso tra 0 e 10
// - Suggerimento: utilizzare lo status recuperato tramite waitpid per "ritornare" il valore calcolato.

// Vi ricordo che l'X-esimo numero di Fibonacci si calcola nel seguente modo:
// f(X) = f(X-1) + f(X-2)
// Sequenza iniziale: 0 1 1 2 3 5 8 13 ...
// In particolare f(0) = 0; f(1) = 1, f(2) = f(0) + f(1) = 0 + 1 = 1 ... 

// Esempio:
// bash:~$ ./fibonacci 7
// 13

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

pid_t PID;

int fib(int N) {

    if (N == 0) {
        return 0;
    } 

    if (N == 1) {
        return 1;
    }

    PID = fork();

    if (PID > 0) { // padre
        int status;
        if (waitpid(PID, &status, 0) != -1) {
            int retSon = WEXITSTATUS(status);
            if (retSon == 255) { // 255 non è un valore che rientra nel codominio della nostra funzione percio se ritorno 255 da un fork implica che ha preso il ramo else di errore nella fork controllo che non mi piace
                exit(-1);
            }
            return fib(N-1) + retSon;
        }    
    } else if (PID == 0) { // figlio
        exit(fib(N-2));
    } else {
        fprintf(stderr, "[ERRORE]: errore nella fork\n");
        exit(-1);
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[ERRORE]: Numero parametri errato\n");
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);

    if (N <= 10 && N >= 0) {
        int fibo = fib(N);
        fprintf(stdout, "result: %d\n", fibo);
    } else {
        fprintf(stdout, "[ERRORE]: il numero in input deve essere compreso fra 0 e 10\n");
        exit(EXIT_FAILURE);
    }

}
