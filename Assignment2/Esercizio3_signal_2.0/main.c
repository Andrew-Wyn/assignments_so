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

#include "utilities.h"

void* fn_cabina(void* args) {

    buffer->fermata_attuale = STAZIONE; 

    while (1) {
    
        Pthread_mutex_lock(&buffer->mtx);

        buffer->in_viaggio = 0;
     
        // avviso che le persone in attesa sulla fermata attuale possono salire
        if (buffer->fermata_attuale == STAZIONE) {
            printf("[CABINA]: partenza dalla stazione\n");
            Pthread_cond_broadcast(&buffer->cond_staz);
        } else if (buffer->fermata_attuale == CENTRO_STORICO) {
            printf("[CABINA]: partenza dal centro storico\n");
            Pthread_cond_broadcast(&buffer->cond_cc);
        }

        // aspetto che siano saliti tutti
        printf("[CABINA]: aspetto che salgano tutti\n");
        // se la cabina non è piena mi metto in attesa finche non sale l'ultimo passeggero l'essere ultimo è gestito dai passegeri stessi
        while (!((buffer->ubriachi == 1 && buffer->num_passengers == 2) || (buffer->sobri == 1 && buffer->num_passengers == 1))){
            Pthread_cond_wait(&buffer->cond_piena, &buffer->mtx);
        }
        
        printf("[CABINA]: partita\n");
        
        print_passengers(buffer);

        // tempo necessario per il viaggio
        sleep(2);

        if (buffer->fermata_attuale == STAZIONE) {
            buffer->fermata_attuale = CENTRO_STORICO;
        } else if (buffer->fermata_attuale == CENTRO_STORICO) {
            buffer->fermata_attuale = STAZIONE;
        }

        // pulisco le variabili inconsistenti per la nuova fermata
        buffer->ubriachi = 0;
        buffer->sobri = 0;
        buffer->in_viaggio = 1;

        if (buffer->fermata_attuale == STAZIONE) {
            printf("[CABINA]: arrivato alla stazione\n");
        } else if (buffer->fermata_attuale == CENTRO_STORICO) {
            printf("[CABINA]: arrivato al centro storico\n");
        }

        // avviso le persone che possono scendere, i processi in attesa sulla cond_arrivata sono tutti e i soli quelli che sono in viaggio
        Pthread_cond_broadcast(&buffer->cond_arrivata);

        // aspetto che siano scesi tutti
        while (buffer->num_passengers != 0)
            Pthread_cond_wait(&buffer->cond_scesi_tutti, &buffer->mtx);

        printf("[CABINA]: sto ripartendo\n");
        
        Pthread_mutex_unlock(&buffer->mtx);
    }   

    pthread_exit(NULL);
    return NULL;
}

void* fn_sobri(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    while (1) {
        Pthread_mutex_lock(&buffer->mtx);

        // il thread si mette in attesa solo se la cabina è in viaggio o se non puo entrare poiche non rispetterebbe i vincoli impostati dal problema
        while (biglietto_->fermata_ != buffer->fermata_attuale || buffer->sobri == 1 || buffer->num_passengers == 2 || buffer->in_viaggio) {
            if (biglietto_->fermata_ == STAZIONE) {
                Pthread_cond_wait(&buffer->cond_staz, &buffer->mtx);
            } else if (biglietto_->fermata_ == CENTRO_STORICO){
                Pthread_cond_wait(&buffer->cond_cc, &buffer->mtx);
            }    
        }

        // modifico le variabili che indicano la salita nella cabina dell'attuale passegero
        buffer->ids[buffer->num_passengers] = biglietto_->id;
        buffer->num_passengers++;
        buffer->ubriachi = 1; // flag per bloccare l'accesso agli ubriachi

        // segnalo alla cabina che puo partire
        if (buffer->num_passengers == 2) {
            Pthread_cond_signal(&buffer->cond_piena);
        }

        printf("[SOBRIO: %d] entro nella cabina\n", biglietto_->id);

        // aspetto l'arrivo della cabina 
        while (!buffer->in_viaggio)
            Pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
        
        printf("[SOBRIO %d] sceso\n", biglietto_->id);

        buffer->num_passengers--;
        // se è l'ultimo a scendere segnalo alla cabina che sono scesi tutti
        if(buffer->num_passengers == 0){
            printf("scesi tutti\n");
            Pthread_cond_signal(&buffer->cond_scesi_tutti);
        }

        // cambio la stazione del passeggero
        if (biglietto_->fermata_ == STAZIONE) {
            biglietto_->fermata_ = CENTRO_STORICO;
        } else if (biglietto_->fermata_ == CENTRO_STORICO) {
            biglietto_->fermata_ = STAZIONE;
        }

        Pthread_mutex_unlock(&buffer->mtx);
        sleep(2);

    }

    pthread_exit(NULL);
    return NULL;
}

void* fn_ubriachi(void* _biglietto) {

    biglietto* biglietto_ = (biglietto*) _biglietto;

    while (1) {
        Pthread_mutex_lock(&buffer->mtx);

        // il thread si mette in attesa solo se la cabina è in viaggio o se non puo entrare poiche non rispetterebbe i vincoli impostati dal problema
        while (biglietto_->fermata_ != buffer->fermata_attuale || buffer->ubriachi == 1 || buffer->num_passengers > 0 || buffer->in_viaggio) {
            if (biglietto_->fermata_ == STAZIONE) {
                Pthread_cond_wait(&buffer->cond_staz, &buffer->mtx);
            } else if (biglietto_->fermata_ == CENTRO_STORICO){
                Pthread_cond_wait(&buffer->cond_cc, &buffer->mtx);
            }    
        }
        
        // modifico le variabili che indicano la salita nella cabina dell'attuale passegero
        buffer->ids[0] = biglietto_->id;
        buffer->num_passengers = 1;
        buffer->sobri = 1;

        printf("[UBRIACO: %d] entro nella cabina\n", biglietto_->id);
        // segnalo che la cabina puo partire
        Pthread_cond_signal(&buffer->cond_piena);

        // aspetto che la cabina arrivi a destinazione
        while (!buffer->in_viaggio)
            Pthread_cond_wait(&buffer->cond_arrivata, &buffer->mtx);
        
        printf("[UBRIACO %d]: sceso\n", biglietto_->id);
        buffer->num_passengers--;
        // se è l'ultimo a scendere segnalo alla cabina che sono scesi tutti cosa sempre vera se visto che è un ubriaco
        if(buffer->num_passengers == 0){
            printf("scesi tutti\n");
            Pthread_cond_signal(&buffer->cond_scesi_tutti);
        }

        // cambio la stazione del passeggero
        if (biglietto_->fermata_ == STAZIONE) {
            biglietto_->fermata_ = CENTRO_STORICO;
        } else if (biglietto_->fermata_ == CENTRO_STORICO) {
            biglietto_->fermata_ = STAZIONE;
        }
        
        Pthread_mutex_unlock(&buffer->mtx);
        sleep(3);
        
    }

    pthread_exit(NULL);
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

        // crea effettivamente in memoria un oggetto shared memory passandogli il nome SHRMEM e le flag per la creazione e per la creazione solo se non preesistente
        shmfd = Shm_open(SHRMEM, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);

        // aggiusto le dimensioni della porzione di memoria necessaria e allocata per lo shared memory object
        Ftruncate(shmfd, shared_seg_size);

        // mappa l'oggeto dalla memoria condivisa allo spazio di indirizzi della virtuale del processo in lettura e scrittura con modificatore MAP_SHARED che implica che le modifiche saranno condivise fra tutti i processi
        buffer = (buffer_condiviso*)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

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

        // inizializzo le variabili nel buffer che devono avere valori specifici dall'inizio
        buffer->num_passengers = 0;
        buffer->ubriachi = 0;
        buffer->sobri = 0;
        buffer->in_viaggio = 1;

        // eseguo cabina
        Pthread_create(&thread_cabina, NULL, fn_cabina, NULL);
        pthread_join(thread_cabina, NULL);

    } else if (role == 'S') { // sobri

        // 6 sobri
        pthread_t* threads_sobri;

        threads_sobri = (pthread_t*)  Malloc(sizeof(pthread_t)*6);

        // creo l'oggetto in memoria condivisa, non viene effettivamente creato da tale processo, ma viene aperto in lettura e scrittura
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        // mappa l'oggeto dalla memoria condivisa allo spazio di indirizzi della virtuale del processo in lettura e scrittura con modificatore MAP_SHARED che implica che le modifiche saranno condivise fra tutti i processi
        buffer = (buffer_condiviso *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*6);

        biglietti[0].id = 1;
        biglietti[0].fermata_ = STAZIONE;
        biglietti[1].id = 2;
        biglietti[1].fermata_ = STAZIONE;
        biglietti[2].id = 3;
        biglietti[2].fermata_ = STAZIONE;
        biglietti[3].id = 4;
        biglietti[3].fermata_ = STAZIONE;
        biglietti[4].id = 5;
        biglietti[4].fermata_ = CENTRO_STORICO;
        biglietti[5].id = 6;
        biglietti[5].fermata_ = CENTRO_STORICO;

        // eseguo i thread dei sobri
        for (int i=0; i<6; i++)
            Pthread_create(threads_sobri + i, NULL, fn_sobri, (void *)(biglietti + i));
        
        for (int i=0; i<6; i++){
            pthread_join(threads_sobri[i], NULL);
        }
        
        // free heap memory
        free(threads_sobri);
        free(biglietti);

    } else if (role == 'U') { // ubriachi

        // 2 ubriachi
        pthread_t* threads_ubriachi;

        threads_ubriachi = (pthread_t*) malloc(sizeof(pthread_t)*2);

        // creo l'oggetto in memoria condivisa, non viene effettivamente creato da tale processo, ma viene aperto in lettura e scrittura
        shmfd = Shm_open(SHRMEM, O_RDWR, S_IRWXU);

        // mappa l'oggeto dalla memoria condivisa allo spazio di indirizzi della virtuale del processo in lettura e scrittura con modificatore MAP_SHARED che implica che le modifiche saranno condivise fra tutti i processi
        buffer = (buffer_condiviso *)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        
        biglietto* biglietti;
        biglietti = Malloc(sizeof(biglietti)*2);

        biglietti[0].id = 7;
        biglietti[0].fermata_ = STAZIONE;
        biglietti[1].id = 8;
        biglietti[1].fermata_ = CENTRO_STORICO;

        // eseguo i threads degli ubriachi
        for (int i=0; i<2; i++)
            Pthread_create(threads_ubriachi + i, NULL, fn_ubriachi, (void *)(biglietti + i));

        for (int i=0; i<2; i++){
            pthread_join(threads_ubriachi[i], NULL);
        }

        // free heap memory
        free(threads_ubriachi);
        free(biglietti);

    } else {
        return 1;
    }

    pthread_exit(NULL);
    return 0;
}