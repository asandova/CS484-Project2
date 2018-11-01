CS484 project 2
    Client-Server reliable UDP trasfer

    To compile us provided makefile
        To compile server type "make serverDriver" on a linux box
        To compile client type "make clientDriver" on a linux box

    To run:
        for Server type "./server (command line args)"
        for client type "./client (command line args)"

        Both programs need command line argument to function correctly
        Both programs need the port, file, ip or host set
            Arguments:
                -port
                    Server: defines which port the server will be binded to
                    client: tell which port is the server lisening prot
                -file:
                    Server: is the file to transmited
                    Client: is the file name to save the transfered data
                -v : sets verbose flag (works for both server and client)
                -debug: sets debug flag (works for both server and client)
                -ip: (client only) is the servers ip address
                -host: (client only) is servers host name
NOTES:
If data packets are being sent, but client or server is not receving packets, 
with ip address and port number correct. Check firewall settings.

To terminate server, use Control^C. I have implemented a function that will catch the control^c interupt and will do a mostly clean exit.
The same is for the client, but is not nessary as the client will ternimate after all data is transfered or too many time outs