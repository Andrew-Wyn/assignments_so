/* 

A Perugia, per andare dalla stazione al centro storico la minimetro' funge da mezzo di spostamento.
Consideriamo una sola cabina con capienza 4 persone e due sole fermate: Stazione, CentroStorico.

Ci sono due tipi di persone: i sobri e gli ubriachi.
Nella cabina viaggiano esattamente 4 sobri per volta e nessun ubriaco e' ammesso.
Gli ubriachi viaggiano da soli.
Ogni volta che la cabina deve partire:
- se il primo che entra e' sobrio allora si deve impedire agli ubriachi di entrare
- se il primo che entra e' ubriaco la cabina parte con il solo ubriaco a bordo

Cabina: Ripete la procedura 0-4 all'infinito
0) Avvisa le persone in attesa su quella fermata che possono salire
1) Attende salita 4 sobri o 1 ubriaco (vedi sopra)
1.1) quando la cabina e' piena stampare "composizione cabina": stampare su stdout gli id delle persone a bordo (ubriachi o sobri)
2) Si sposta sull'altra fermata (impiegando 2 secondi)
2.1) la cabina stampa su stdout che sta ripartendo
2.2) stampare su stdout arrivo alla fermata Stazione/CentroStorico
3) Avvisa le persone che possono scendere
3.1) quando una persona scende scrive su stdout che e' sceso
4) Aspetta che siano scesi tutti

Le stampe su stdout elencate sopra sono obbligatorie, potete includerne altre a vostra scelta che aiutino a capire lo svolgimento dello scenario.

Sobri e ubriachi: 
Una persona dopo essere scesa dalla cabina "fa un giro" di 2 secondi (i sobri) oppure 3 secondi (gli ubriachi) e poi si rimette in coda per tornare sull'altra fermata. Ripete questo procedimento all'infinito.

Inizializzazione:
- La cabina parte dalla fermata Stazione
- Ci sono 7 persone in totale
- All'inizio si hanno:
-- Stazione: 4 sobri e 1 ubriaco
-- CentroStorico: 1 sobrio e 1 ubriaco

Implementazione:
1) Implementare il sistema descritto, utilizzando dei PROCESSI per modellare Cabina, Sobrio e Ubriaco (3 processi distinti)
2) Utilizzare uno shared memory object
2.1) --NON-- sono ammessi altri strumenti di comunicazione tra processi, e.g., pipe
2) Utilizzate le opportune strutture dati per la sincronizzazione
3) Il progetto deve essere costituito da almeno 3 file (scegliete voi i nomi oppurtuni): 
3.1) file contenente il main e le funzioni Cabina, Sobrio e Ubriaco, e.g., minimetro.c
3.2) file contenente le chiamate a funzione di libreria/SC con relativa gestione dell'errore, e.g., utilities.c utilities.h, oppure se definite tramite macro un unico file utilities.h
3.3) Makefile
4) Gestite eventuali errori nelle chiamate a funzione di libreria/SC. In caso di errore grave, terminare il programma producendo un avviso a video dell'errore.
5) Scrivere il Makefile per compilare e linkare i sorgenti. La mancanza del Makefile viene considerato un errore grave.
5.1) Il makefile deve contenere anche il target "clean"

[UPDATE 8/05/2020]

La configurazione iniziale precedente poteva portare ad una situazione di stallo (vedi post su forum annunci per spiegazione completa).

Nuova configurazione iniziale per esercizio BONUS:

Sobri totale 6

Ubriachi totali 2

Capienza massimo della cabina diventa 2



Stazione alla partenza ha 4 sobri e 1 ubriaco

Centro storico alla partenza ha 2 sobri e 1 ubriaco

*/



// AGGIUNGERE FUNZIONE PER PULIRE BUFFER prima dell'avvio dei thread

// TEST

#define _GNU_SOURCE


#include "utilities.h"

buffer_condiviso* buffer;

void print_all_stat_buff(buffer_condiviso* buffer) {
    printf("-----------\n");

    printf("passengers: %d\n", buffer->num_passengers);
    printf("flag sobri %d\n", buffer->sobri);
    printf("flag ubriachi %d\n", buffer->ubriachi);
    printf("sobri centro storico %d\n", buffer->sobri_cc);
    printf("sobri stazione %d\n", buffer->sobri_stazione);
    printf("pronta %d\n", buffer->pronta);

    printf("-----------\n");
    fflush(stdout);
}

void* fn_cabina(void* args) {
    printf("cabina\n");

    // 0: stazione, 1: centro storico
    buffer->fermata_attuale = 0; 

    while (1) {
    
        printf("CABINA: DICO CHE CI STO\n");
        Pthread_mutex_lock(&buffer->mtx);
        //printf("--------------locko\n");
        buffer->pronta=1; // nuovo avviso tramite flag
        printf("CABINA: ASPETTO CHE SI RIEMPIA\n");
        //printf("--------------slocko\n");
        Pthread_cond_wait(&buffer->cond_piena, &buffer->mtx);
        //printf("--------------locko\n");

        if (buffer->fermata_attuale) {
            printf("CABINA: partita dal centro storico\n");
        } else {
            printf("CABINA: Arrivato dalla stazione\n");
        }

        buffer->pronta=0;

        fflush(stdout);

        print_passengers(buffer);

        // tempo necessario per il viaggio
        sleep(2);
        
        Pthread_cond_signal(&buffer->cond_arrivata);
        
        if (buffer->fermata_attuale) {
            printf("CABINA: Arrivato alla stazione\n");
            buffer->fermata_attuale = 0;
        } else {
            printf("CABINA: Arrivato al centro storico\n");
            buffer->fermata_attuale = 1;
        }

        //printf("--------------slocko\n");
        Pthread_cond_wait(&buffer->cond_scesi_tutti, &buffer->mtx);
        //printf("--------------locko\n");

        printf("sto ripartendo\n");

        buffer->ubriachi = 0;
        buffer->sobri = 0;
        buffer->num_passengers = 0;

        //printf("--------------slocko\n");
        Pthread_mutex_unlock(&buffer->mtx);
    }   

    return NULL;
}

void* fn_sobri(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    int num_sobri;

    printf("SOBRIO: %d\n", biglietto_->id);
    
    while (1) {
        Pthread_mutex_lock(&buffer->mtx);
        //printf("--------------sobrio locko\n");



        if ((buffer->pronta) && (biglietto_->fermata_ == buffer->fermata_attuale)) {

            if (buffer->fermata_attuale == 0) {
                num_sobri = buffer->sobri_stazione;
            } else {
                num_sobri = buffer->sobri_cc;
            }

            if (buffer->ubriachi == 0 && buffer->num_passengers < 2 && num_sobri >= 2) {
                buffer->ids[buffer->num_passengers] = biglietto_->id;
                buffer->num_passengers++;
                buffer->sobri = 1;
                if (buffer->num_passengers == 2) {
                    Pthread_cond_signal(&buffer->cond_piena);
                }
                printf("SOBRIO: %d -> ENTRO NELLA CABINA, siamo in %d\n", biglietto_->id, buffer->num_passengers);
                print_all_stat_buff(buffer);
                //printf("--------------slocko\n");
                Pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
                //printf("--------------locko\n");
                printf("SONO SOBRIO E SCESO: %d\n", biglietto_->id);

                buffer->num_passengers--;
                if(buffer->num_passengers == 0){
                    printf("tutti fuori\n");
                    Pthread_cond_signal(&buffer->cond_scesi_tutti);
                } else {
                    Pthread_cond_signal(&buffer->cond_arrivata);
                }
                // diminuire il contatore dei sobri;
                if (biglietto_->fermata_) {
                    buffer->sobri_stazione++;
                    buffer->sobri_cc--;
                    biglietto_->fermata_ = 0;
                } else {
                    buffer->sobri_stazione--;
                    buffer->sobri_cc++;
                    biglietto_->fermata_ = 1;
                }
                fflush(stdout);
                //printf("--------------slocko\n");
                Pthread_mutex_unlock(&buffer->mtx);
                sleep(2);
            } else {
                //printf("--------------slocko\n");
                Pthread_mutex_unlock(&buffer->mtx);
            }
        } else {
            //printf("--------------slocko\n");
            Pthread_mutex_unlock(&buffer->mtx);
        }
        pthread_yield(); 
    }

    return NULL;
}

void* fn_ubriachi(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    printf("UBRIACO: %d\n", biglietto_->id);

    // 0: stazione, 1: centro storico

    //printf("--------------locko\n");
    //printf("--------------slocko\n");

    while (1) {
        Pthread_mutex_lock(&buffer->mtx);
        //printf("--------------ubriaco locko\n");
        
        if ((buffer->pronta) && (biglietto_->fermata_ == buffer->fermata_attuale)) {
            if (buffer->sobri == 0 && buffer->ubriachi == 0) {
                buffer->ids[0] = biglietto_->id;
                buffer->num_passengers = 1;
                buffer->ubriachi = 1;
                printf("UBRIACO: %d -> ENTRO NELLA CABINA, siamo in %d\n", biglietto_->id, buffer->num_passengers);
                fflush(stdout);
                Pthread_cond_signal(&buffer->cond_piena);
                //printf("--------------slocko\n");
                Pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
                //printf("--------------locko\n");
                printf("SONO UBRIACO E SCESO: %d\n", biglietto_->id);
                buffer->num_passengers--;
                if(buffer->num_passengers == 0){
                    pthread_cond_signal(&buffer->cond_scesi_tutti);
                }
                if (biglietto_->fermata_) {
                    buffer->ubriachi_stazione++;
                    buffer->ubriachi_cc--;
                    biglietto_->fermata_ = 0;
                } else {
                    buffer->ubriachi_stazione--;
                    buffer->ubriachi_cc++;
                    biglietto_->fermata_ = 1;
                }
                fflush(stdout);
                //printf("--------------slocko\n");
                Pthread_mutex_unlock(&buffer->mtx);
                sleep(3);
            } else {
                //printf("--------------slocko\n");
                Pthread_mutex_unlock(&buffer->mtx);
            }
            
        } else {
            //printf("--------------slocko\n");
            Pthread_mutex_unlock(&buffer->mtx);
        }
        
        pthread_yield(); 
    }

    return NULL;
}

int main(int argc, char* argv[]) {

    int shmfd;
    int shared_seg_size = sizeof(buffer_condiviso);

    char role;

    if (argc != 2) {
        fprintf(stderr, "[ERRORE]: passare argomenti C, S o U\n");
    } else if (strcmp(argv[1], "C") == 0){
        role = 'C';
    } else if (strcmp(argv[1], "S") == 0){
        role = 'S';
    } else if (strcmp(argv[1], "U") == 0){
        role = 'U';
    } else {
        fprintf(stderr, "[ERRORE]: passare argomenti C, S o U\n");
    }

    if (role == 'C') { /* cabina */
        pthread_t thread_cabina;
        // init mem cond
        shmfd = shm_open(SHRMEM, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
        if (shmfd < 0) {
            perror("shm_open");
            exit(1);
        }

        if (ftruncate(shmfd, shared_seg_size) != 0) {
            perror("ftruncate");
            exit(1);
        }

        buffer = (buffer_condiviso*)mmap(NULL, sizeof(buffer_condiviso), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

        // inizializzo i mutex e variabili condizionali con relativi attributi
        pthread_mutexattr_t mattr;
        pthread_condattr_t cvattr;

        Pthread_mutexattr_init(&mattr);

        Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&buffer->mtx, &mattr);

        Pthread_condattr_init(&cvattr);

        Pthread_condattr_setpshared(&cvattr, PTHREAD_PROCESS_SHARED);

        Pthread_cond_init(&buffer->cond_staz, &cvattr);

        Pthread_cond_init(&buffer->cond_cc, &cvattr);

        Pthread_cond_init(&buffer->cond_piena, &cvattr);

        Pthread_cond_init(&buffer->cond_arrivata, &cvattr);

        Pthread_cond_init(&buffer->cond_scesi_tutti, &cvattr);

        Pthread_condattr_destroy(&cvattr);

        Pthread_mutexattr_destroy(&mattr);


        buffer->num_passengers = 0;
        buffer->ubriachi = 0;
        buffer->sobri = 0;
        buffer->sobri_stazione=4;
        buffer->sobri_cc=2;
        buffer->ubriachi_stazione=1;
        buffer->ubriachi_cc=1;

        /* eseguo cabina */
        Pthread_create(&thread_cabina, NULL, fn_cabina, NULL);

    } else if (role == 'S') { // sobri

        // 4 sobri
        pthread_t* threads_sobri;

        threads_sobri = (pthread_t*)  Malloc(sizeof(pthread_t)*6);

        /* Inizializzazione segmento di memoria condivisa */
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        buffer = (buffer_condiviso *)mmap(NULL, sizeof(buffer_condiviso), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*6);

        biglietti[0].id = 1;
        biglietti[0].fermata_ = 0;
        biglietti[1].id = 2;
        biglietti[1].fermata_ = 0;
        biglietti[2].id = 3;
        biglietti[2].fermata_ = 0;
        biglietti[3].id = 4;
        biglietti[3].fermata_ = 0;
        biglietti[4].id = 5;
        biglietti[4].fermata_ = 1;
        biglietti[5].id = 6;
        biglietti[5].fermata_ = 1;

        for (int i=0; i<6; i++)
            Pthread_create(threads_sobri + i, NULL, fn_sobri, (void *)(biglietti + i));
            
        for (int i=0; i<2; i++){
            pthread_join(threads_sobri[i], NULL);
        }

        free(threads_sobri);
        threads_sobri = NULL;
        free(biglietti);
        biglietti = NULL;

    } else if (role == 'U') { // ubriachi

        // 2
        pthread_t* threads_ubriachi;

        threads_ubriachi = (pthread_t*) malloc(sizeof(pthread_t)*2);

        /* Inizializzazione segmento di memoria condivisa */
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        buffer = (buffer_condiviso *)mmap(NULL, sizeof(buffer_condiviso), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*2);

        biglietti[0].id = 7;
        biglietti[0].fermata_ = 0;
        biglietti[1].id = 8;
        biglietti[1].fermata_ = 1;

        /* eseguo produttore */
        for (int i=0; i<2; i++)
            Pthread_create(threads_ubriachi + i, NULL, fn_ubriachi, (void *)(biglietti + i));

        for (int i=0; i<2; i++){
            pthread_join(threads_ubriachi[i], NULL);
        }

        free(threads_ubriachi);
        threads_ubriachi = NULL;
        free(biglietti);
        biglietti = NULL;

    } else {
        return 1;
    }

    pthread_exit(NULL);
    return 0;
}