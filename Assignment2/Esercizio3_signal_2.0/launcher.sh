#! /bin/bash
var_shMem=minimetro
unlink /dev/shm/$var_shMem &> /dev/null

echo -e "Compilo ...\n"
make
if [ $? -eq 0 ]; then
    echo -e "\nCompilazione terminata correttamente\n"
else
    echo -e "\nCompilazione fallita. Rimozione eventuali file generati:"
    # make clean
    exit
fi

echo -e "Creo Cabina"
./main C &
pid_cabina=$!

sleep 1

echo -e "\nControllo esistenza /dev/shm/${var_shMem}:"
if ! ls -lh /dev/shm/ | grep -w $var_shMem; then
    echo -e "Errore: non trovo l'area di memoria"
    exit
fi

sleep 2

echo -e "Creo Sobri"
./main S &
pid_sobri=$!

echo -e "Creo Ubriahi"
./main U &
pid_ubriachi=$!

sleep 10

kill -9 ${pid_cabina}
kill -9 ${pid_sobri}
kill -9 ${pid_ubriachi}

wait ${pid_cabina}
echo risultato wait $?
wait ${pid_sobri}
echo risultato wait $?
wait ${pid_ubriachi}
echo risultato wait $?
# il risultato dovrebbe essere 137 (128 + 9)

unlink /dev/shm/$var_shMem &> /dev/null