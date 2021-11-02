# RAFT Concensus Algorithm for Log Replication

Project realized for the ALGOREP teaching at EPITA.

Authors :

- Jérémy d'Auria
- Maxime Perrin-Livenais
- Nicolas Guerduadj
- Youssef Ouhmmou

Supervised by:

- Etienne Renault

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

The command list per client is automaticaly generated in the script.

## Using the REPL

When running you can interact with the system, here is the list of valid commands:

- `START <CLIENT_TARGET>`: Start the client rank
- `CRASH <SERVER_TARGET>`: Crash the server
- `SPEED <SPEED_OPTION> <SERVER_TARGET>`: Set the speed of the server
  - Valid `<SPEED_OPTION>`: `LOW, MEDIUM, HIGH`

### Advice

If you want to split the logs and the REPL, you can redirect the output to an other terminal:

```sh
# In the other terminal, get the tty value
$ tty
/dev/pts/1

# On the first one
$ ./launch.sh > /dev/pts/1
```
