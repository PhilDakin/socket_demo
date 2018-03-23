// Compile: g++ server.cpp -lsocket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    
    /*
       Create socket
       
       AF_INET     - IPv4 Internet Protocol
       SOCK_STREAM - reliable, 2-way byte stream

       returns -1 on error
    */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("opening stream socket");
        exit(1);
    }
    

    /*
       Create struct for storing network metadata

       sin_family - specify an address family (e.g., IPv4 Internet Protocol)
       s_addr     - specify a network address (INADDR_ANY if we don't care)
       sin_port   - specify a port number     (0 if we don't care)
    */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;


    /*
       This is not C++-11 binding! This is a Linux system call within <sys/socket>.
       Bind the socket descriptor (sock) to the address we've specified (server).
       This is also called "naming a socket".

       returns -1 on error
    */
    if (bind(sock, (struct sockaddr *) &server, sizeof server) == -1) {
        perror("binding stream socket");
        exit(1);
    }

    
    /* 
       We didn't specify a port number, so we got whatever the system gave us.
       getsockname lets us figure out what that port is.

       returns -1 on error
    */
    unsigned length = sizeof server;
    if (getsockname(sock,(struct sockaddr *) &server,&length) == -1) {
        perror("getting socket name");
        exit(1);
    }
    
    printf("Socket port #%d\n", ntohs(server.sin_port));
    

    /*
       Start listening for client requests. Second argument specifies the size
       of the backlog queue of client requests.
    */
    listen(sock, 5);


    /*
       Accept a single request. msgsock is the "name" of the client socket.

       This only accepts a single request.
         - How would we process multiple?
         - How would we process them all at the same time?

       returns -1 on error
    */
    unsigned msgsock = 0;
    msgsock = accept(sock,(struct sockaddr *) 0,(unsigned *) 0);
    if (msgsock == -1) {
        perror("accept");
        exit(1);
    }

    
    /*
       Call receive until we have finished reading the client's request.
       Each call to receive produces [1, min(msgsize, bufsize)] bytes.
         - Given the following code, how do you tell if you recieved the end
           of the message?
    */
    char buf[1024];
    int rval = 0;
    do {
        memset(buf, 0, sizeof buf);
        if ((rval = recv(msgsock, buf, 1024, 0)) == -1)
            perror("reading stream message");
        if (rval == 0)
            printf("Ending connection\n");
        else
            printf("-->%s\n", buf);

        //printf("%d\n", rval);
    } while (rval != 0);


    /*
       Close our connection with the socket
         - Frees the descriptor for reuse
         - Terminate both sides of connection
    */
    close(msgsock);


    return 0;
}