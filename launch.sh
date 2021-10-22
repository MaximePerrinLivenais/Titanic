read -p 'Choose the number of servers: ' nb_servers
read -p 'Choose the number of clients: ' nb_clients

nb_process=$((1 + nb_servers + nb_clients))

echo "localhost slots=$nb_process" > hostfile

mpirun -hostfile hostfile --mca opal_warn_on_missing_libcuda 0 ./titanic $nb_servers $nb_clients
