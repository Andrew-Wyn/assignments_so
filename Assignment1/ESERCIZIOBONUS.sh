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
valori_totali=() # mantengo tutti i valori trovati in ogni file per calcolarmi poi le statistiche totali
numero_righe_tot=0
somma_tot=0
media_tot=0
deviazione_standard_tot=0

function process_file() {
    # parametri
    local file_="$1"

    # variabili di funzione
    local numero_righe=0
    local somma=0
    local media=0
    local deviazione_standard=0

    # !) tutte le operazioni vengono effettuate tramite il tool bc visto che bash non supporta i floating point e cio viene fatto tramite command expansion $()
    # !) le regex utilizzate, oltre che nel debugging dell'esercizio, sono state testate tramite il sito regex101

    local id_row
    local value_row
    local regex_number='(^[0-9]+$)|(^[0-9]+[.][0-9]*$)|(^[0-9]*[.][0-9]+$)' # regex che fa match solo con valori
    local id_value_regex='^[0-9]*$' # regex che fa match solo con numeri interi per controllare gli indici nella prima colonna
    local values_=() # variabile che utilizzero come array dove salvero i valori della riga, mi servira per calcolare la deviazione standard

    while read row_ || [[ -n "$row_" ]]; do # row_ contiene una riga del file passato come input alla funzione, il secondo controllo viene effettuato per leggere anche l'ultima riga se il file non termina con \n
        # splitting della riga in due variabili tramite comando cut per utilizzarlo devo passare sullo stdout il contenuto della righa
        id_row=$(echo $row_ | cut -d" " -f1)
        value_row=$(echo $row_ | cut -d" " -f2)

        if ! [[ $id_row =~ $id_value_regex ]]; then # controllo che il valore presente come indice della riga sia numerico intero positivo
            echo -e "ERRORE: valore non numerico intero positivo presente nella prima colonna -> ($id_row)"
            exit 1
        fi

        if [[ $value_row =~ $regex_number ]]; then # =~ effettua un match tra la stringa a sinistra e una regex sulla sinistra, torna 0 se il match avviene necessario doppio [] per tornare valore booleano
            somma=$(echo "scale=2; $somma+$value_row" | bc -q)
            values_[$numero_righe]=$value_row
            valori_totali[$numero_righe_tot]=$value_row

            ((numero_righe ++)) # calcolo la lunghezza delle righe aumentando un contatore
            ((numero_righe_tot ++)) # calcolo il numero delle righe totati progressivamente
        else
            echo -e "ERRORE: è presente un valore non numerico nella seconda colonna -> ($value_row)"
            exit 1
        fi
    done < "$file_" # leggo riga per riga (fino a EOF) prendendo come stdin il file contenuto nella variabile $file (preso spunto dall'esempio nelle sue slide)

    # MEDIA
    # calcolo media -> somma/n_righe
    media=$(echo "scale=2; $somma/$numero_righe" | bc -q)

    # DEVIAZIONE STANDARD
    # solo dopo aver calcolato la media per definizione posso calcolare la dev standard
    for val_ in "${values_[@]}"; do
        deviazione_standard=$(echo "scale=2; $deviazione_standard+($val_-$media)^2" | bc -q)
    done
    deviazione_standard=$(echo "scale=2; sqrt($deviazione_standard/($numero_righe-1))" | bc -q)

    # mi calcolo la somma totale di tutti i valori presenti in tutti i file per il calcolo finale
    somma_tot=$(echo "scale=2; $somma_tot+$somma" | bc -q)

    # STAMPO VALORI PER OGNI FILE 
    # echo -e "$(basename -s .csv $file_), $numero_righe, $somma, $media, $deviazione_standard" # elimino il suffisso dal file se ha estensione .csv
    echo -e "$(echo $file_ | cut -d "." -f1), $numero_righe, $somma, $media, $deviazione_standard" # taglio rispetto al punto e prendo la substring iniziale, meno performante ma adatto per gestire tutte le estenzioni
}

if [[ $# -ge 1 ]]; then

    for par_ in "$@"; do
        if [[ -f $par_ ]]; then
            process_file "$par_" # uso il quoting per evitare errori con nomi dei file che contengono spazi
        else
            echo -e "ERRORE: Inserire Solo file, '${par_}' non è un file"
            exit 1
        fi 
    done

    # calcolo valori totali vedo tutti i valori come se appartenessero ad un unico file

    # MEDIA
    # calcolo media_tot -> somma/n_righe
    media_tot=$(echo "scale=2; $somma_tot/$numero_righe_tot" | bc -q)

    #DEVIAZIONE STANDARD
    # solo dopo aver calcolato la media per definizione posso calcolare la dev standard
    for val_ in "${valori_totali[@]}"; do
        deviazione_standard_tot=$(echo "scale=2; $deviazione_standard_tot+($val_-$media_tot)^2" | bc -q)
    done

    deviazione_standard_tot=$(echo "scale=2; sqrt($deviazione_standard_tot/($numero_righe_tot-1))" | bc -q)

    # STAMPO I RISULTATI FINALI
    echo -e "TOT, $numero_righe_tot, $somma_tot, $media_tot, $deviazione_standard_tot"

else
    echo -e "ERRORE: Numero parametri sbagliati"
    exit 1
fi

