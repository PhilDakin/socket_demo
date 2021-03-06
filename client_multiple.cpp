// Compile: g++ client.cpp -lsocket -lnsl
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <iostream>


/*
* This program creates a socket and initiates a connection with the
* socket given in the command line. The user is given an input interface..
* functioning chatroom. 
*/
int main(int argc, char *argv[]) {
    /*
       Create socket
       
       AF_INET     - IPv4 Internet Protocol
       SOCK_STREAM - reliable, 2-way byte stream

       returns -1 on error
    */
    int sock = socket( AF_INET, SOCK_STREAM, 0 );
    if (sock == -1) {
        perror("opening stream socket");
        exit(1);
    }
    

    /* Connect socket using name specified by command line. */
    struct sockaddr_in server;
    server.sin_family = AF_INET;


    /*
       Connect to an open socket with name (descriptor) specified
       on command-line

       returns a structure including the network address
       of the specified host

       returns 0 on error
    */
    struct hostent* hp = gethostbyname(argv[1]);
    
    if (hp == (struct hostent *) 0) {
        fprintf(stderr, "%s: unknown host\n", argv[1]);
        exit(2);
    }


    /*
       Let the localhost socket be our server socket. Connect to it via
       command-line argument port number.

       htons - host to network short

       Why is this necessary? Network byte ordering is often different,
       so we need to convert to allow the network to read our data properly.

       N.B. This is not necessary for the data we are sending over the network.
    */
    memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));


    /*
       Connect to the server's socket

       returns -1 on error
    */
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("connecting stream socket");
        exit(1);
    }


    while(true){
        std::string in;
        printf("Enter your message (less than 1024 bytes!!): ");
        getline(std::cin, in);

        int sent_bytes = 0;
        int desired_bytes = strlen(in.c_str()) + 1;

        while(sent_bytes != desired_bytes){
          int sent = send(sock, in.c_str(), strlen(in.c_str()) + 1 - sent_bytes, 0);
          sent_bytes += sent;
        }
    }
    /*
       Close our connection with the socket
         - Frees the descriptor for reuse
         - Terminate both sides of connection
    */
    close(sock);


    return 0;
}