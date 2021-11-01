read -p 'Choose the number of servers: ' nb_servers
read -p 'Choose the number of clients: ' nb_clients
read -p 'Number of request per clients: ' nb_requests



# Create folder where servers while save logs
if [ -d "server_logs" ]; then
    rm server_logs -rf
fi
mkdir "server_logs"

# Create folder where clients will load commands
if [ -d "client_commands" ]; then
    rm client_commands -rf
fi
mkdir "client_commands"


nb_process=$((1 + nb_servers + nb_clients))
first_client=$((nb_servers + 1))
for client in $(seq "$first_client" "$nb_process"); do
    filename="client_commands/commands_$client.txt"
    for req_idx in $(seq $nb_requests); do
        echo "Client $client - command $req_idx" >> $filename
    done
done


echo "localhost slots=$nb_process" > hostfile

#mpirun -hostfile hostfile --mca opal_warn_on_missing_libcuda 0 ./titanic $nb_servers $nb_clients # &> output.txt
mpirun -hostfile hostfile --mca opal_warn_on_missing_libcuda 0  xterm -e gdb --args ./titanic $nb_servers $nb_clients
