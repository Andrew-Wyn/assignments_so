#! /bin/bash

# 4) Scrivere uno script bash che prende "k>=2" nomi di file di ingresso e appende i primi "k-1" files nel file passato come 
# ultimo argomento (k-esimo), scrivendo i file in ingresso nell'ordine da destra a sinistra

# Esempio: 
# myscript.sh file1 file2 file3 file4
# -> file4 conterra' in sequenza file3 file2 file1

# SUGGERIMENTO: creare un array contenente tutti gli argomenti passati allo script.

if [[ $# -ge 2 ]]; then

    # inserisco i nomi dei file in dall'ultimo al primo di inserimento in un array
    # Array creation
    i=$(( $# - 1 )) # variabile indice per scorrere l'array in senso decrescente
    
    for par_ in "$@"; do # necessario mettere il quoting senno nomi passati come stringa unica ex "nome file1" viene splittato casistica particolare ma possibile
        
        if [[ -f $par_ ]]; then # controllo che i file inseriti siano effettivamente file
            files_[$i]=$par_
            ((i--))
        else 
            echo -e "ERRORE: inserire solamente file, ${par_} non è un file"
            exit 1
        fi
    done

    unset i # rimuovo la variabile i che non mi serve piu

    # scorro l'array dal penultimo inserito al primo in ordine decrescente
    for file_ in "${files_[@]:1 }"; do
        echo -e "appendo il file ${file_} al file ${files_[0]} ..."
        cat "$file_" >> "${files_[0]}" # appendo il file che sto leggendo al file 0 nell'array ovvero l'ultimo letto

        # controllo se l'esecuzione degli apppend avviene senza errori
        append_exit_status=$?
        if ! [[ $append_exit_status -eq 0 ]]; then # controllo se l'exit status è diverso da 0
            echo -e "ERRORE: errore nell appending dei file -> error code (${append_exit_status})"
            exit 1
        fi

    done 

    echo -e "\nDone!!!"
else 
    echo -e "ERRORE: il numero dei file deve essere >= 2"
    exit 1
fi 