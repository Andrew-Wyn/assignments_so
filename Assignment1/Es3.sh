#! /bin/bash

# 3) Scrivere uno script bash con il seguente funzionamento:
# - Riceve un e un solo argomento, in caso contrario restituisce un errore
# - Se l'argomento non è una directory restituisce errore
# - Se l'argomento è una directory crea un archivio tar.gz con il nome della directory data in input
# - Se il comando di creazione dell'archivio non è andato a buon fine restituisce un errore
# - Se il comando di creazione ha avuto successo stampa sullo std output il contenuto dell'archivio compresso
# - Se l'archivio è già esistente chiede all'utente se vuole sovrascriverlo: in caso negativo esce, in caso positivo elimino l'archivio vecchio e lo ricrea

# Esempio:
# bash~$ ls dir1/
# file1.txt file2.txt
# bash~$ ./crea_archivio.sh
# Utilizzo di questo script: crea_archivio.sh nomedirectory
# bash~$ ./crea_archivio.sh dir1 dir2
# Utilizzo di questo script: crea_archivio.sh nomedirectory
# bash~$ ./crea_archivio.sh dir1
# creo l'archivio dir1.tar.gz
# archivio creato con successo, il contenuto e':
# dir1/
# dir1/file1.txt
# dir1/file2.txt
# bash~$ ./crea_archivio.sh dir1
# il file dir1.tar.gz esiste gia' sovrascriverlo (S/N)?



function crea_archivio() {

    # utilizzo delle variabili locali e non faccio riferimento alla stessa variabile globale di terminale, per una migliore leggibilita
    local nome_archivio=$1
    local file_path=$2

    tar -czf $nome_archivio $file_path # creo archivio con il comando tar ed le opzioni, -c create, -z compress con gzip, -f nome dell'archivio
    tar_exit_status=$? # prendo exit status del comando precedente

    # controllo se il return status di tar sia zero in caso contrario lo ritorno come errore e segnalata in stdout
    if [ $tar_exit_status -eq 0 ]; then 
        echo -e "Archivio creato correttamente, il contenuto è il seguente:"
        tar -ztf $nome_archivio # visualizzo il contenuto dell'archivio senza decomprimerlo, con l'opzione -t
        else
        echo -e "ERRORE: creazione archivio non andata a buon fine -> error code (${tar_exit_status})"
        exit 1
    fi
    
}

if [ $# -eq 1 ]; then # controllo che il numero dei parametri (escluso il parametro $0) sia uguale a 1

    # utilizzo i path relativi per i file
    file_path=$1

    if [ -d $file_path ]; then

        nome_archivio=${file_path}.tar.gz

        if [ -f $nome_archivio ]; then # controllo che il path relativo dell'archivio esista e sia un file

            # leggo risposta dall'utente
            echo -e "Archivio Esistente, Vuoi sovrascriverlo?[S/n]"
            read risp_ # leggo una linea dal buffer stdin

            if [ $risp_ == "S" ] || [ $risp_ == "s" ]; then 
                crea_archivio $nome_archivio $file_path
            else
                echo -e "Hai detto di no"
            fi

        else
            crea_archivio $nome_archivio $file_path
        fi

    else
        echo -e "ERRORE: L'Input non è riferito ad una directory"
        exit 1
    fi

else 
    echo -e "ERRORE: Numero parametri errato, richiesti: 1"
    exit 1
fi