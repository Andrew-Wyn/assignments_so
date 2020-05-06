/* A Perugia, per andare dalla stazione al centro storico la minimetro' funge da mezzo di spostamento.
// Consideriamo una sola cabina con capienza 4 persone e due sole fermate: Stazione, CentroStorico.

// Ci sono due tipi di persone: i sobri e gli ubriachi.
// Nella cabina viaggiano esattamente 4 sobri per volta e nessun ubriaco e' ammesso.
// Gli ubriachi viaggiano da soli.
// Ogni volta che la cabina deve partire:
// - se il primo che entra e' sobrio allora si deve impedire agli ubriachi di entrare
// - se il primo che entra e' ubriaco la cabina parte con il solo ubriaco a bordo

// Cabina: Ripete la procedura 0-4 all'infinito
// 0) Avvisa le persone in attesa su quella fermata che possono salire
// 1) Attende salita 4 sobri o 1 ubriaco (vedi sopra)
// 1.1) quando la cabina e' piena stampare "composizione cabina": stampare su stdout gli id delle persone a bordo (ubriachi o sobri)
// 2) Si sposta sull'altra fermata (impiegando 2 secondi)
// 2.1) la cabina stampa su stdout che sta ripartendo
// 2.2) stampare su stdout arrivo alla fermata Stazione/CentroStorico
// 3) Avvisa le persone che possono scendere
// 3.1) quando una persona scende scrive su stdout che e' sceso
// 4) Aspetta che siano scesi tutti

// Le stampe su stdout elencate sopra sono obbligatorie, potete includerne altre a vostra scelta che aiutino a capire lo svolgimento dello scenario.

// Sobri e ubriachi: 
// Una persona dopo essere scesa dalla cabina "fa un giro" di 2 secondi (i sobri) oppure 3 secondi (gli ubriachi) e poi si rimette in coda per tornare sull'altra fermata. Ripete questo procedimento all'infinito.

// Inizializzazione:
// - La cabina parte dalla fermata Stazione
// - Ci sono 7 persone in totale
// - All'inizio si hanno:
// -- Stazione: 4 sobri e 1 ubriaco
// -- CentroStorico: 1 sobrio e 1 ubriaco

// Implementazione:
// 1) Implementare il sistema descritto, utilizzando dei PROCESSI per modellare Cabina, Sobrio e Ubriaco (3 processi distinti)
// 2) Utilizzare uno shared memory object
// 2.1) --NON-- sono ammessi altri strumenti di comunicazione tra processi, e.g., pipe
// 2) Utilizzate le opportune strutture dati per la sincronizzazione
// 3) Il progetto deve essere costituito da almeno 3 file (scegliete voi i nomi oppurtuni): 
// 3.1) file contenente il main e le funzioni Cabina, Sobrio e Ubriaco, e.g., minimetro.c
// 3.2) file contenente le chiamate a funzione di libreria/SC con relativa gestione dell'errore, e.g., utilities.c utilities.h, oppure se definite tramite macro un unico file utilities.h
// 3.3) Makefile
// 4) Gestite eventuali errori nelle chiamate a funzione di libreria/SC. In caso di errore grave, terminare il programma producendo un avviso a video dell'errore.
// 5) Scrivere il Makefile per compilare e linkare i sorgenti. La mancanza del Makefile viene considerato un errore grave.
// 5.1) Il makefile deve contenere anche il target "clean"
*/

// TEST

#include "utilities.h"

buffer_condiviso* buffer;

void* fn_cabina(void* args) {
    printf("cabina\n");

    // 0: stazione, 1: centro storico
    int fermata_attuale = 0; 

    while (1) {
        printf("CABINA: ASPETTO CHE TUTTI SIANO IN ATTESA\n");
        Pthread_mutex_lock(&buffer->mtx);
        Pthread_cond_wait(&buffer->cond_waitfull, &buffer->mtx);
        
        printf("CABINA: TUTTI IN ATTESA\n");

        if (fermata_attuale == 0) {
            printf("CABINA: SEGNALO STAZIONE\n");
            Pthread_cond_signal(&buffer->cond_staz);
        } else if (fermata_attuale == 1) {
            printf("CABINA: SEGNALO CENTRO STORICO\n");
            Pthread_cond_signal(&buffer->cond_cc);
        } else {
            // errore
        }
        printf("CABINA: ASPETTO CHE SI RIEMPIA\n");
        Pthread_cond_wait(&buffer->cond_piena, &buffer->mtx);

        print_passengers(buffer);

        // tempo necessario per il viaggio
        sleep(2);

        if (fermata_attuale) {
            fermata_attuale = 0;
        } else {
            fermata_attuale = 1;
        }

        buffer->ubriachi = 0;

        printf("sto ripartendo\n");
        Pthread_cond_broadcast(&buffer->cond_arrivata);
        Pthread_cond_wait(&buffer->cond_scesi_tutti, &buffer->mtx);
        Pthread_mutex_unlock(&buffer->mtx);
    }   

    return NULL;
}

void* fn_sobri(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    int num_sobri;

    while (1) {
        Pthread_mutex_lock(&buffer->mtx);
        buffer->waitlen++;
        if(buffer->waitlen >= 4) // !!! // capire perche va in deadlock quando non aspetto esattamente tutti i passeggeri
            Pthread_cond_signal(&buffer->cond_waitfull);
        if (biglietto_->fermata_ == 0) {
            printf("SOBRIO: %d -> aspetto alla stazione\n", biglietto_->id);
            buffer->sobri_stazione++; // !!!
            Pthread_cond_wait(&buffer->cond_staz, &buffer->mtx);
            num_sobri = buffer->sobri_stazione;
        } else if (biglietto_->fermata_ == 1) {
            buffer->sobri_cc++; // !!!
            printf("SOBRIO: %d -> aspetto al centro storico\n", biglietto_->id);
            Pthread_cond_wait(&buffer->cond_cc, &buffer->mtx);
            num_sobri = buffer->sobri_cc;
        } else {
            // errore
        }

        buffer->waitlen--;

        if (buffer->num_passengers < 4 && num_sobri >= 4) {
            buffer->ids[buffer->num_passengers] = biglietto_->id;
            buffer->num_passengers++;
            buffer->ubriachi = 1;
            if (buffer->num_passengers == 4) {
                Pthread_cond_signal(&buffer->cond_piena);
            } else {
                if (biglietto_->fermata_ == 0) {
                    Pthread_cond_signal(&buffer->cond_staz);
                } else if (biglietto_->fermata_ == 1) {
                    Pthread_cond_signal(&buffer->cond_cc);
                } else {
                    // errore
                }
            }
            printf("SOBRIO: %d -> ENTRO NELLA CABINA\n", biglietto_->id);
            Pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
            printf("SONO SOBRIO E SCESO: %d\n", biglietto_->id);

            buffer->num_passengers--;
            if(buffer->num_passengers == 0){
                printf("tutti fuori\n");
                Pthread_cond_signal(&buffer->cond_scesi_tutti);
            }
            // diminuire il contatore dei sobri;
            if (biglietto_->fermata_) {
                //buffer->sobri_stazione++;
                buffer->sobri_cc--;
                biglietto_->fermata_ = 0;
            } else {
                buffer->sobri_stazione--;
                //buffer->sobri_cc++;
                biglietto_->fermata_ = 1;
            }

            Pthread_mutex_unlock(&buffer->mtx);
            sleep(2);
        } else {
            
            if (biglietto_->fermata_ == 0) {
                Pthread_cond_signal(&buffer->cond_staz);
            } else if (biglietto_->fermata_ == 1) {
                Pthread_cond_signal(&buffer->cond_cc);
            } else {
                // errore
            }
            Pthread_mutex_unlock(&buffer->mtx);
        }
    }

    return NULL;
}

void* fn_ubriachi(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    printf("UBRIACO: %d\n", biglietto_->id);

    // 0: stazione, 1: centro storico

    while (1) {
        pthread_mutex_lock(&buffer->mtx);
        buffer->waitlen++;
        if(buffer->waitlen >= 0) // !!!
            pthread_cond_signal(&buffer->cond_waitfull);
        if (biglietto_->fermata_ == 0) {
            printf("UBRIACO: %d -> aspetto alla stazione\n", biglietto_->id);
            pthread_cond_wait(&buffer->cond_staz, &buffer->mtx);
        } else if (biglietto_->fermata_ == 1) {
            printf("UBRIACO: %d -> aspetto al centro storico\n", biglietto_->id);
            pthread_cond_wait(&buffer->cond_cc, &buffer->mtx);
        } else {
            // errore
        }

        buffer->waitlen--;

        if (buffer->ubriachi == 0) {
            buffer->ids[0] = biglietto_->id;
            buffer->num_passengers = 1;
            buffer->ubriachi = 1;
            printf("UBRIACO: %d -> ENTRO NELLA CABINA\n", biglietto_->id);
            pthread_cond_signal(&buffer->cond_piena);
            pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
            printf("SONO UBRIACO E SCESO: %d\n", biglietto_->id);
            buffer->num_passengers--;
            if(buffer->num_passengers == 0){
                pthread_cond_signal(&buffer->cond_scesi_tutti);
            }
            if (biglietto_->fermata_) {
                biglietto_->fermata_ = 0;
            } else {
                biglietto_->fermata_ = 1;
            }
            pthread_mutex_unlock(&buffer->mtx);
            sleep(3);
        } else {
            if (biglietto_->fermata_ == 0) {
                Pthread_cond_signal(&buffer->cond_staz);
            } else if (biglietto_->fermata_ == 1) {
                Pthread_cond_signal(&buffer->cond_cc);
            } else {
                // errore
            }
            pthread_mutex_unlock(&buffer->mtx);
        }
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

        Pthread_cond_init(&buffer->cond_waitfull, &cvattr);

        Pthread_condattr_destroy(&cvattr);

        Pthread_mutexattr_destroy(&mattr);


        buffer->num_passengers = 0;
        buffer->ubriachi = 0;
        buffer->waitlen = 0;
        buffer->sobri_stazione=4;
        buffer->sobri_cc=1;

        /* eseguo cabina */
        Pthread_create(&thread_cabina, NULL, fn_cabina, NULL);

    } else if (role == 'S') { // sobri

        // 4 sobri
        pthread_t* threads_sobri;

        threads_sobri = (pthread_t*)  Malloc(sizeof(pthread_t)*5);

        /* Inizializzazione segmento di memoria condivisa */
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        buffer = (buffer_condiviso *)mmap(NULL, sizeof(buffer_condiviso), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*5);

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

        for (int i=0; i<5; i++)
            Pthread_create(threads_sobri + i, NULL, fn_sobri, (void *)(biglietti + i));

    } else if (role == 'U') { // ubriachi

        // 2
        pthread_t* threads_ubriachi;

        threads_ubriachi = (pthread_t*) malloc(sizeof(pthread_t)*2);

        /* Inizializzazione segmento di memoria condivisa */
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        buffer = (buffer_condiviso *)mmap(NULL, sizeof(buffer_condiviso), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*2);

        biglietti[0].id = 6;
        biglietti[0].fermata_ = 0;
        biglietti[1].id = 7;
        biglietti[1].fermata_ = 1;

        /* eseguo produttore */
        for (int i=0; i<2; i++)
            Pthread_create(threads_ubriachi + i, NULL, fn_ubriachi, (void *)(biglietti + i));

    } else {
        return 1;
    }

    pthread_exit(NULL);
    return 0;
}