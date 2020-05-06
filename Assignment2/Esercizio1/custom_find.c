// Realizzare comando custom_find che cerca ricorsivamente i file con una 
// certa estensione nel sottoalbero radicato nella directory eventualmente passata come argomento (vedi sotto). 
// Per ogni file trovato, stampa il path assoluto della directory in cui e' stato trovato e la data dell'ultima modifica.
// Il programma riceve al piu' due argomenti in input: 
// 1) stringa estensione, e.g., ".txt" [primo argomento e obbligatorio]
// 2) stringa path [secondo argomento, facoltativo]: se l'argomento c'e' 
// si cerca nel sottoalbero radicato nella directory "path", se non c'e' si cerca a partire da "."

// Si implementi un codice C che utilizza chiamate di System Call per poter eseguire il suo compito.

// Esempio:
// bash:~$ ./custom_find ".txt" "directory"
// /home/fcoro/directory/file1.txt Fri May  5 19:25:09 2020
// /home/fcoro/directory/subdir/file2.txt Fri May  4 19:25:09 2020
// bash:~$ ./custom_find ".txt"
// /home/fcoro/file3.txt Fri May  5 19:25:09 2020
// /home/fcoro/directory/file1.txt Fri May  5 19:25:09 2020
// /home/fcoro/directory/subdir/file2.txt Fri May  4 19:25:09 2020

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h> // exit, EXIT_FAILURE
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAXBUFF 128

int checkExt(char* fileName, const char* extStr) {
    int lenFileName = strlen(fileName);
    int lenExtStr = strlen(extStr);
    if (lenExtStr > lenFileName) {
        return 0;
    }
    for (int i=lenFileName-lenExtStr; i<=lenFileName-1; i++) {
        if (fileName[i] != extStr[i + lenExtStr - lenFileName]) {
            return 0;
        }
    }
    return 1;
}

int checkValidDir(char* fileName) {
    return (strlen(fileName) > 0 && fileName[strlen(fileName) - 1] != '.');
}

void recursiveSearchDir(const char* nomeDir, const char* extStr) {

    struct stat statFile;
    int retVal;
    int varErrno;

    if ((retVal = stat(nomeDir, &statFile)) != 0) {
        varErrno = errno;
        perror("stat");
        exit(varErrno);
    }

    if (!S_ISDIR(statFile.st_mode)) {
        fprintf(stderr, "[ERRORE]: non è una directory valida");
        exit(EXIT_FAILURE);
    }

    DIR* dir;

    if ((dir = opendir(nomeDir)) == NULL) {
        varErrno = errno;
        perror("opendir");
        exit(EXIT_FAILURE);
    }


    struct dirent* file = NULL;

    while ((errno = 0, file = readdir(dir)) != NULL) {
        char fileName[MAXBUFF] = "";

        if (strlen(nomeDir) + strlen(file->d_name) + 2 > MAXBUFF) {
            fprintf(stderr, "ERRORE MAXBUFF TROPPO PICCOLO\n");
            exit(EXIT_FAILURE);
        }

        strncpy(fileName, nomeDir, MAXBUFF -1);
        if (nomeDir[strlen(nomeDir)-1] != '/')
            strncat(fileName, "/", MAXBUFF -1);
        strncat(fileName, file->d_name, MAXBUFF -1);

        if ((retVal = stat(fileName, &statFile)) != 0) {
            varErrno = errno;
            perror("stat");
            exit(varErrno);
        }

        if (S_ISDIR(statFile.st_mode)) {
            if (checkValidDir(fileName)){
                recursiveSearchDir(fileName, extStr);
            }
                
        } else {
            if (checkExt(fileName, extStr)) 
                printf("nome file: %s - ultima modifica: %s", realpath(fileName, NULL), ctime(&statFile.st_mtime));
        }
    }

    if (errno != 0) {
        perror("readdir");
    }

    if ((retVal = closedir(dir)) != 0) {
        varErrno=errno;
        perror("closedir");
        exit(varErrno);
    }

}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "numero parametro non corretto\n");
        exit(EXIT_FAILURE);
    }

    char nomeDir[MAXBUFF] = "";

    if (argc == 2) {
        strcpy(nomeDir, "./");
    } else {
        strcpy(nomeDir, argv[2]);
        // controllo se è una cartella
    }

    recursiveSearchDir(nomeDir, argv[1]);

}