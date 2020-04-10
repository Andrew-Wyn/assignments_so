#! /bin/bash

# 1) Estrarre (e stampare sullo standard output) dal file /etc/passwd login-name e home (primo e sesto campo) come segue:
# - solo i login-name che hanno la home in '/home/'
# - ordine alfabetico

# Esempio:
# alice /home/alice
# bob /home/bob
# pippo /home/pippo

# Per Risolvere questo esercizio ho utilizzato i seguenti comandi:
# - cut 
# - grep 
# - tr
# - sort
# inoltre li ho eseguiti tramite pipe, ovvero in modo sequenziale e in modo tale che l'input del comando successivo sia l'output del precedente
# nella soluzione proposta effettuo una cut e sul file '/etc/passwd' e ritorno i field 1 e 6 per ogni riga delimitati dal carattere 
# ':' poi di tale output prendo solo le righe che contengono la parola '/home/' che seleziona gli utenti la cui home è in '/home', 
# di queste ultime linee sostituisco i caratteri ':' con ' ' per formattare l'output
# in modo affine alla richesta. Per ultimo faccio un sorting delle linee ottenute fin ora e passate tramite stdin a sort stesso
# cat /etc/passwd | cut -d ":" -f1,6 | grep "/home/" | tr ":" " " | sort

cut -d ":" -f1,6 /etc/passwd | grep "/home/" | tr ":" " " | sort