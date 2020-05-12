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
#include <stdlib.h> // exit, EXIT_FAILURE, realpath
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>

#define MAXBUFF 128

// aggiungere wrapping functions per le systemcalls 

// WRAPPED FUNCTION FOR ERROR HANDLING

void Stat(const char* nomeDir, struct stat* statFile) {
    int retVal;
    if ((retVal = stat(nomeDir, statFile)) != 0) {
        int varErrno = errno;
        perror("stat");
        exit(varErrno);
    }
}

DIR* Opendir(const char* nomeDir) {
    DIR* dir;
    if ((dir = opendir(nomeDir)) == NULL) {
        int varErrno = errno;
        perror("opendir");
        exit(varErrno);
    }
    return dir;
}

void Closedir(DIR* dir) {
    int retVal;
    if ((retVal = closedir(dir)) != 0) {
        int varErrno = errno;
        perror("closedir");
        exit(varErrno);
    }
}

// UTILITIES FUNCTIONS

// passato un filename e un'estenzione controllo se il file name ha tale estensione 1 true 0 false
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

// controllo che il file name non si riferisce a una directory . o ..
int checkValidDir(char* fileName) {
    return (strlen(fileName) > 0 && fileName[strlen(fileName) - 1] != '.');
}

void recursiveSearchDir(const char* nomeDir, const char* extStr) {
    struct stat statFile;
    DIR* dir;
    struct dirent* file = NULL;

    // effettuo stat sul path passato in input nella funzione
    Stat(nomeDir, &statFile);

    // se il file associato al path non è una directory ritorno errore (notare che questo controllo puo andare in exit solo durante la prima call dal main)
    if (!S_ISDIR(statFile.st_mode)) {
        fprintf(stderr, "[ERRORE]: non è una directory valida\n");
        exit(EXIT_FAILURE);
    }
    
    // uso della SC per aprire la directory, il path è riferito relativo rispetto alla cartella da dove si richiama l'eseguibile
    dir = Opendir(nomeDir);

    // ciclo sui file contenuti nella directory tramite la SC readdir
    while ((errno = 0, file = readdir(dir)) != NULL) {
        char fileName[MAXBUFF] = ""; // stringa che conterra il nome della cartella attuale + il nome dei file che contiene 

        // se la MACRO MAXBUFF è troppo piccola per contenere il path dell'attuale file ritorna errore
        if (strlen(nomeDir) + strlen(file->d_name) + 2 > MAXBUFF) {
            fprintf(stderr, "[ERRORE]: MAXBUFF sistuisce un valore troppo piccolo\n");
            exit(EXIT_FAILURE);
        }

        // concateno il nome del file al nome della directory della ricorsione
        strncpy(fileName, nomeDir, MAXBUFF -1);
        if (nomeDir[strlen(nomeDir)-1] != '/')
            strncat(fileName, "/", MAXBUFF -1);
        strncat(fileName, file->d_name, MAXBUFF -1);

        // effettuo una stat sul nuovo path relativo
        Stat(fileName, &statFile);

        if (S_ISDIR(statFile.st_mode)) { // se anche esso è una directory controllo che non siano la rid . o la .. e ricorro nella stessa
            if (checkValidDir(fileName))
                recursiveSearchDir(fileName, extStr);
        } else { // se non è una direcotory stampo le caratteristiche richieste realpath e data di ultima modifica
            if (checkExt(fileName, extStr)){  // prima di stampare controllo che il nome del file abbia l'estensione richiesta
                /*
                    according to man 3 realpath:
                    If  resolved_path  is  specified as NULL, then realpath() uses malloc(3) to 
                    allocate a buffer of up to PATH_MAX bytes to hold the resolved pathname, and returns a pointer to this buffer.  The caller
                    should deallocate this buffer using free(3).
                */
                char* realPathFileName = realpath(fileName, NULL);
                printf("nome file: %s - ultima modifica: %s", realPathFileName, ctime(&statFile.st_mtime));
                free(realPathFileName);
            }
        }
    }

    if (errno != 0) {
        perror("readdir");
    }
    
    Closedir(dir);

}

int main(int argc, char *argv[]) {
    char nomeDir[MAXBUFF];

    if (argc == 1) {
        fprintf(stderr, "[ERRORE]: numero parametri non corretto\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        strcpy(nomeDir, "./");
    } else if (argc == 3) {
        strcpy(nomeDir, argv[2]);
    } else {
        fprintf(stderr, "[ERRORE]: numero parametri non corretto\n");
        exit(EXIT_FAILURE);
    }

    recursiveSearchDir(nomeDir, argv[1]);
    return 0;
}