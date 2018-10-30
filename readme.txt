CS484 project 2
    Client-Server reliable UDP trasfer

    To compile us provided makefile
        To compile server type "make serverDriver" on a linux box
        To compile client type "make clientDriver" on a linux box

    To run:
        for Server type "./server (command line args)"
        for client type "./client (command line args)"

        Both programs take command line arguments
            Arguments:
                -port
                    Server: defines which port the server will be binded to
                    client: tell which port is the server lisening prot
                -file:
                    Server: is the file to tranmited
                    Client: is the file name to save the transfered data
                -v : sets verbose flag (works for both server and client)
                -debug: sets debug flag (works for both server and client)
                -ip: (client only) is the servers ip address
