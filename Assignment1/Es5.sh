#! /bin/bash

# 5) Scrivere uno script bash che dati 2 file di testo in input (f1 ed f2) produce il seguente menu di scelta per l'utente:
# - 1. rimuovere entrambi i file
# - 2. archiviare entrambi i file
# - 3. appendere il file f1 al file f2
# - 4. esci

# - Implementate ogni funzione del menù sopra
# - Per opzione 1 chiedere conferma prima della rimozione
# - Per opzione 2 produrre l'archivio compresso f1f2.tar.gz (il nome è dato dai due nomi dei file concatenati)
# - NON utilizzare il builtin select per implementare il menu

# preferisco trattare i nomi dei file come parametri delle singole funzioni e trattarli poi come variabili locali per una migliore lettura

function delete_both() {

    local file1_="$1"
    local file2_="$2"

    echo -e "ELIMINAZIONE\nSicuro di voler procedere?[S/n]"
    local risp_
    read risp_

    if [[ $risp_ == "S" ]] || [[ $risp_ == "s" ]]; then

        rm "$file1_" "$file2_" # rimuovo il file1 e il file 2
        # controllo il successo dell'operazione
        rm_exit_status=$? # get exit status del comando precedente
        if [[ $rm_exit_status -eq 0 ]]; then
            echo -e "File Eliminati!!"
        else
            echo -e "ERRORE: durante l'eliminazione dei file -> cod err(${rm_exit_status})"
            exit 1
        fi
        
    else 
        echo -e "Annullata Operazione"
    fi
}

function archive_both() {
    echo -e "ARCHIVIAZIONE"

    local file1_="$1"
    local file2_="$2"

    local nome_archivio=$(basename "$file1_")$(basename "$file2_").tar.gz # il nome dell'archivio è dato dal basename dei file tolgo i prefissi
    # local nome_archivio=$(basename "$file1_" | cut -d "." -f1)$(basename "$file2_" | cut -d "." -f1).tar.gz # dei nomi dei file prendo tutto tranne il suffisso (cio che viene dopo il primo punto) e il prefisso
    tar -czf  "$nome_archivio" "$file1_" "$file2_" # creo archivio con il comando tar ed le opzioni, -c create, -z compress con gzip, -f nome dell'archivio
    # controllo il successo dell'operazione
    tar_exit_status=$? # get exit status del comando precedentes
    if [[ $tar_exit_status -eq 0 ]]; then
        echo -e "Archivio creato correttamente, il contenuto è il seguente:"
        tar -ztf "$nome_archivio" # visualizzo il contenuto dell'archivio senza decomprimerlo, con l'opzione -t
    else
        if [[ -e "$nome_archivio" ]] && [[ -z $(tar -ztf "$nome_archivio") ]];then # operazione eseguita per evitare di trovare archivio vuoto creato ma non riempito causa errore del comando tar
            rm "$nome_archivio" 
        fi
        echo -e "ERRORE: durante la creazione dell'archivio -> cod err(${tar_exit_status})"
        exit 1
    fi 
}

function append_1_2() {

    local file1_="$1"
    local file2_="$2"

    echo -e "APPEND"

    cat "$file1_" >> "$file2_"
    # controllo il successo dell'operazione
    append_exit_status=$? # get exit status del comando precedente
    if [[ $tar_exit_status -eq 0 ]]; then
        echo -e "Append avvenuto correttamente!!"
    else
        echo -e "ERRORE: durante l'effettuazione dell'append -> cod err(${tar_exit_status})"
        exit 1
    fi 

}

if [[ $# -eq 2 ]]; then # controlo che i file inseriti siano esattamete 2

    file1_="$1"
    file2_="$2"

    if ! [[ -f "$file1_" ]] || ! [[ -f "$file2_" ]]; then 
        echo -e "ERRORE: Inserire solo file"
        exit 1
    fi

    while true; do
        echo -e "### MENU ###"

        # leggo la risposta dallo standard input
        echo -e "- 1. rimuovere entrambi i file\n- 2. archiviare entrambi i file\n- 3. appendere il file f1 al file f2\n- 4. esci" 
        read risp_ # leggo una linea dal buffer stdin

        echo $risp_ 

        case $risp_ in 
            1)
                delete_both "$file1_" "$file2_" # preferisco passare il valore dei nomi dei file come parametri per una migliore lettura del codice
                ;;
            2)
                archive_both "$file1_" "$file2_"
                ;;
            3)
                append_1_2 "$file1_" "$file2_"
                ;;
            4)
                break
                ;;
        esac
    done

else
    echo -e "ERRORE: Numero parametri sbagliato, vanno inseriti due valori in input"
    exit 1
fi