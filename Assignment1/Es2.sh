#! /bin/bash

# 2) Stampare la lista di tutti i nomi di file contenuti nella propria home 
# che sono stati modificati negli ultimi 2 minuti e che contengono al loro interno la parola 'cookies'


# Per risolvere questo esercizio ho utilizzato i seguenti comandi:
# - find
# - grep
# ho utilizzato il comando find dalla cartella home (espansa tramite carattere ~) e con i filtri -mmin -2  (file modificati al massimo due minuti da ora) e -type f (solo file), poi per ogni 
# file matchato applico il comando grep -l "cookies" che mette in stdout il nome del file se contiene la parola cercata

find ~ -mmin -2 -type f -exec grep -l "cookies" \{\} \;

# !) nella mia macchina è necessario eseguire il comando con permessi di amministratore per avere accesso ad alcune cartelle 