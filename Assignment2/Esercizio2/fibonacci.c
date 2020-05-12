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
#include <errno.h>

pid_t PID;
// funzione di fibonacci ricorsiva la quale associa ad ogni sottochiamata di se stessa al flusso di un processo differente padre e figlio
// si noti che l'albero della ricorsione fermato in un qualsiasi punto ha nelle foglie processi differenti che attendono il wait degli altri tranne quello
// che è attualmente in esecuzione
int fib(int N) {

    // casi base della ricorsione, non serve generare figli per calcolarli
    if (N == 0) {
        return 0;
    } 

    if (N == 1) {
        return 1;
    }

    PID = fork();

    if (PID > 0) { // padre
        int status;
        if (waitpid(PID, &status, 0) != -1) { // aspetto return del processo figlio che sta eseguendo la fib (n-2) prima di richiamare la fib (n-1) e risplittare l'esecuzione
            int retSon = WEXITSTATUS(status);
            if (retSon == 255) { // 255 non è un valore che rientra nel codominio della nostra funzione percio se ritorno 255 da un fork implica che ha preso il ramo else di errore nella fork controllo che non mi piace
                exit(-1);
            }
            // solo il figlio di ogni istanza ritornera il valore tramite exit status normalmente il padre in ogni layer ritorna il valore intero uscendo normalmente dalla chiamata, se poi tale chiamata era richiamata da un figlio esso tornera il valore al padre chiamante con una exit
            return fib(N-1) + retSon;
        } else { // waitpid error
            fprintf(stderr, "[ERRORE]: nella waitpid, processo: %d\n", getpid());
            perror("waitpid");
            exit(-1);
        }
    } else if (PID == 0) { // figlio
        exit(fib(N-2)); // richiamo la ricorsione ritornando il valore tramite exit status al padre chiamante
    } else { // gestione errore della fork
        fprintf(stderr, "[ERRORE]: errore nella fork, processo: %d\n", getpid());
        perror("fork");
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[ERRORE]: Numero parametri errato\n");
        exit(EXIT_FAILURE);
    }

    char* end;

    int N = strtol(argv[1], &end, 10);

    if (*end != '\0') {
        fprintf(stdout, "[ERRORE]: non hai inserito un numero\n");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", N);

    if (N <= 10 && N >= 0) {
        int fibo = fib(N);
        fprintf(stdout, "[OK] result: %d\n", fibo);
    } else {
        fprintf(stdout, "[ERRORE]: il numero in input deve essere compreso fra 0 e 10\n");
        exit(EXIT_FAILURE);
    }

}
