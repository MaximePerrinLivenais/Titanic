# Log replication with RAFT consensus algorithm

Project realized for the ALGOREP course of Etienne Renault at EPITA.

Authors :

- Jérémy d'Auria
- Maxime Perrin-Livenais
- Nicolas Guerguadj
- Youssef Ouhmmou

## Requirements

- `libopenmpi-dev`
- C++ compiler supporting C++20

## Build

To build this project, you only have to run this following command:

```sh
$ make
```

## Run the program

To run this program, you can use the `launch.sh` script:

```sh
$ chmod +x launch.sh
$ ./launch.sh
```

Once the script is launched, you have to enter the number of server, client and requests by client.

```sh
# Example

$ ./launch.sh
Choose the number of servers: 18
Choose the number of clients: 3
Number of request per client: 25
```

The command list per client is automaticaly generated in the script and are stored in `client_commands`
directory.

Logs of each server are stored in the `server_logs` directory.

## Using the REPL

When running you can interact with the system, here is the list of valid commands:

- `START <CLIENT_TARGET>`: Start the client rank
- `CRASH <SERVER_TARGET>`: Crash the server
- `SPEED <SPEED_OPTION> <SERVER_TARGET>`: Set the speed of the server
  - Valid `<SPEED_OPTION>`: `LOW, MEDIUM, HIGH`
- `SEND <CLIENT_TARGET>`: Client send one request to servers
- `START <CLIENT_TARGET>`: Start client, it will send all its requests one by one

### Advice

If you want to split the logs and the REPL, you can redirect the output to an other terminal:

```sh
# In the other terminal, get the tty value
$ tty
/dev/pts/1

# On the first one
$ ./launch.sh > /dev/pts/1
```

## Check results

At the end of the execution, you can run

```sh
./check_logs.sh
```

to check if all server logs are identical.
