#! /bin/bash

# ESERCIZIO BONUS: 
# La risoluzione di questo esercizio prevede l'assegnamento di un punto bonus. 
# Tale punto bonus viene assegnato SOLO SE tutti e 6 gli esercizi vengono risolti.
# Se NON risolvete uno o più degli esercizi 1-5 ma viene comunque risolto l'esercizio bonus il punto NON viene assegnato.

# Data in input una lista di file testuali, ognuno dei quali contiene per ogni riga due campi numerici separati da spazio.
# Scrivere uno script bash che legge il secondo campo di ogni file e ne calcola: somma, media e deviazione standard, e stampa sullo std output una stringa con il seguente formato:
# [nomedelfile senza estensione] [numero di linee del file] [somma] [media] [deviazione standard]
# Infine stampa [numero di linee totali] [somma] [media] [deviazione standard] di tutti i file.
# NB: gestite eventuali valori in input che non siano numeri (vedi esempio 2)

# Esempio 1 (NB: valori arbitrari, potrebbero non essere corretti):
# bash~$ cat file1.csv
# 1 20.0
# 2 15.5
# 3 25.3
# bash~$ ./script.sh file1.csv file2.csv
# file1 3 60.8 20.26 4.90
# file2 10 10 5.03 0.05
# TOT 13 70.8 12.6 4.8

# Esempio 2:
# bash~$ cat file1.csv
# 1 20.0
# 2 uno
# 3 25.3
# bash~$ ./script.sh file1.csv
# errore: ho trovato una riga che non conteneva numeri

# SUGGERIMENTO: per effettuare i calcoli in floating point usare il comando bc 
# Esempio: Stampo la radice quadrata di 12 con troncamento a 2 cifre dopo la virgola
# bash~$ echo "scale=2; sqrt(12)" | bc -q
# 3.46

# variabili globali
numero_righe_tot=0
somma_tot=0
media_tot=0
deviazione_standard_tot=0

function process_file() {
    # parametri
    local file_=$1

    # variabili di funzione
    local numero_righe=0
    local somma=0
    local media=0
    local deviazione_standard=0

    # !) tutte le operazioni vengono effettuate tramite il tool bc visto che bash non supporta i floating point e cio viene fatto tramite command expansion $()
    # !) le regex utilizzate, oltre che nel debugging dell'esercizio, sono state testate tramite il sito regex101

    # METODO 1

    # controllo se la prima riga contiene solamente valori numerici interi

    # local id_rows=$(cut -d" " -f1 $file_)
    # local values_=$(cut -d" " -f2 $file_) # non è un array ma l'espansione dell'output del comando cut
    # local id_number_regex='^[0-9]*$' # regex che fa match solo con numeri interi per controllare gli indici nella prima colonna
    # local regex_value='^[0-9]*([.][0-9]*)*$' # regex che fa match solo con valori 

    # # controllo tutti gli elementi output del comando cut che rappresentano la prima colonna come stringa sliced
    # for val_ in $id_rows; do 
    #     if ! [[ $val_ =~ $id_number_regex ]]; then
    #         echo -e "ERRORE: valore non numerico intero positivo presente nella prima colonna -> ($val_)"
    #         exit 1
    #     fi
    # done

    # # passo al calcolo delle statistiche tramite i valori presenti nella seconda colonna

    # # calcolo somma totale
    # for val_ in $values_; do # lo tratto come un insieme di valori sui quali posso iterare, ovvero una stringa che viene scissa seguendo i delimitatori nella variabile IFS
    #     if [[ $val_ =~ $regex_value ]]; then # =~ effettua un match tra la stringa a sinistra e una regex sulla sinistra, torna 0 se il match avviene necessario doppio [] per tornare valore booleano
    #         somma=$(echo "scale=2; $somma+$val_" | bc -q) 
    #         ((numero_righe ++)) # devo calcolarmi il numero di righe iterativamente visto che non posso interrogare la lunghezza dell'output di cut come se fosse un array
    #     else
    #         echo -e "ERRORE: è presente un valore non numerico nella seconda colonna -> ($val_)"
    #         exit 1
    #     fi
    # done

    # METODO 2

    local id_row
    local value_row
    local regex_number='^[0-9]*([.][0-9]*)*$' # regex che fa match solo con valori
    local id_value_regex='^[0-9]*$' # regex che fa match solo con numeri interi per controllare gli indici nella prima colonna
    local i=0
    local values_ # variabile che utilizzero come array dove salvero i valori della riga, mi servira per calcolare la deviazione standard

    while read row_; do # row_ contiene una riga del file passato come input alla funzione
        # splitting della riga in due variabili tramite comando cut per utilizzarlo devo passare sullo stdout il contenuto della righa
        id_row=$(echo $row_ | cut -d" " -f1)
        value_row=$(echo $row_ | cut -d" " -f2)

        if ! [[ $id_row =~ $id_value_regex ]]; then # controllo che il valore presente come indice della riga sia numerico intero positivo
            echo -e "ERRORE: valore non numerico intero positivo presente nella prima colonna -> ($id_row)"
            exit 1
        fi

        if [[ $value_row =~ $regex_number ]]; then # =~ effettua un match tra la stringa a sinistra e una regex sulla sinistra, torna 0 se il match avviene necessario doppio [] per tornare valore booleano
            somma=$(echo "scale=2; $somma+$value_row" | bc -q)
            values_[$i]=$value_row

            ((i ++))
            ((numero_righe ++)) # calcolo la lunghezza delle righe aumentando un contatore
        else
            echo -e "ERRORE: è presente un valore non numerico nella seconda colonna -> ($value_row)"
            exit 1
        fi
    done < $file_ # leggo riga per riga (fino a EOF) prendendo come stdin il file contenuto nella variabile $file (preso spunto dall'esempio nelle sue slide)

    unset i
    

    # calcolo media -> somma/n_righe
    media=$(echo "scale=2; $somma/$numero_righe" | bc)

    # solo dopo aver calcolato la media per definizione posso calcolare la dev standard
    for val_ in ${values_[@]}; do
        deviazione_standard=$(echo "scale=2; $deviazione_standard+($val_-$media)^2" | bc -q)
    done

    deviazione_standard=$(echo "scale=2; sqrt($deviazione_standard/($numero_righe-1))" | bc -q)

    # stampo valori richiesti
    # echo -e "$(basename -s .csv $file_), $numero_righe, $somma, $media, $deviazione_standard" # elimino il suffisso dal file se ha estensione .csv
    echo -e "$(echo $file_ | cut -d "." -f1), $numero_righe, $somma, $media, $deviazione_standard" # taglio rispetto al punto e prendo la substring iniziale, meno performante ma adatto per gestire tutte le estenzioni

    # aggiorno valori globali sommandovi quelli locali
    numero_righe_tot=$(echo "scale=2; $numero_righe_tot+$numero_righe" | bc -q)
    somma_tot=$(echo "scale=2; $somma_tot+$somma" | bc -q)
    media_tot=$(echo "scale=2; $media_tot+$media" | bc -q)
    deviazione_standard_tot=$(echo "scale=2; $deviazione_standard_tot+$deviazione_standard" | bc -q)
}

if [ $# -ge 1 ]; then

    for par_ in $@; do
        if [ -f $par_ ]; then
            process_file $par_
        else
            echo -e "ERRORE: Inserire Solo file"
            exit 1
        fi 
    done

    # # per ogni file preso in input processo i suoi vaolire e li stampo un output, tengo anch delle variabili globali da aggiornare ad ogni iterazione (file passato)
    # # le quali verranno stampate alla fine
    # for file_ in ${files_[@]}; do
    #     process_file $file_
    # done 

    echo -e "TOT, $numero_righe_tot, $somma_tot, $media_tot, $deviazione_standard_tot"

else
    echo -e "ERRORE: Numero parametri sbagliati"
    exit 1
fi

