/*

Realizzare in C un programma client ed un programma server. 
I due processi intergiscono utilizzando socket AF_UNIX.

SERVER:
- gestisce più connessioni alla volta tramite thread
-- per ogni connessione lancia un nuovo thread che gestirà la connessione con il client
- il messaggio di risposta è una stringa uguale al messaggio originario ma ogni carattere ha invertito il "letter case", i.e., caratteri maiuscoli diventano minuscoli e viceversa
-- Esempio: stringa ricevuta "cIAo" -> stringa restituita "CiaO"
-- Potete evitare il controllo di caratteri speciali (non sara' considerato errore)
-- Il server esegue il controllo se la stringa contiene caratteri "white-space" (' ', \f, \t, \r, \t, \v). In caso ne vengano trovati NON esegue la conversione e restituisce un errore al client
-- SUGGERIMENTO: man 3 islower/isupper; man 3 tolower/toupper, man 3 isspace

CLIENT:
- apre una connessione verso il server ed invia una stringa
- attende il risultato prima di inviare una nuova stringa
- termina quando riceve in input (dall'utente) stringa "quit"

Implementare una gestione dei segnali nel server a seguito della ricezione di uno dei seguenti segnali:
- SIGINT, SIGQUIT, SIGTERM, SIGHUP
- Nel caso di ricezione di uno di questi segnali il server deve terminare lasciando lo stato consistente
- Utilizzare un thread dedicato la gestione dei segnali
-- NON si possono utilizzare signal-handlers (solo eventualmente per SIGPIPE se ritenuto opportuno)
- SUGGERIMENTO: man 2 shutdown per la chiusura del socket AF_UNIX
- SUGGERIMENTO: man 3 pthread_attr_setdetachstate


La soluzione proposta deve contenere anche un makefile che deve generare due eseguibili: uno per il server e uno per il client.
Il codice sorgente del cliente e del server deve essere scritto in due file separati.

*/

#if !defined(UTIL_H)
#define UTIL_H

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#define SOCKNAME "./comunication_socket"
#define BUFSIZE 256

#endif