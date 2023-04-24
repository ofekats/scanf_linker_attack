#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define RECEIVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1000

int main()
{
    
    //opening a socket for the sender
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //check if there is no exception
    if (sock == -1) {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    // used for IPv4 communication
    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));

    recv_addr.sin_family = AF_INET; // Address family, AF_INET unsigned 
    recv_addr.sin_port = htons(PORT); // receiver port number 
    recv_addr.sin_addr.s_addr  = INADDR_ANY; // Internet address
    int rval = inet_pton(AF_INET,RECEIVER_IP, &recv_addr.sin_addr);  // convert IPv4 addresses from text to binary form

    //check if there is no exception
    if (rval <= 0) {
        printf("inet_pton() failed");
        return -1;
    } 

    
    // Bind the socket to the port with any IP
    int bindResult = bind(sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr));

	//check if there is no exception
    if (bindResult == -1) {
        printf("Bind failed with error code : %d", errno);
        // close the socket
        close(sock);
        return -1;
    }

    // Make the socket listening
    int listenResult = listen(sock, 1);

    

	//check if there is no exception
    if (listenResult == -1) {
        printf("listen() failed with error code : %d", errno);
        // close the socket
        close(sock);
        return -1;
    }
    printf("receiver is on..\n");
    while(1)
    {
        //Accept and incoming connection
        struct sockaddr_in serverAddress;
        socklen_t serverAddressLen = sizeof(serverAddress);

        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddressLen = sizeof(serverAddress);
        int serverSocket = accept(sock, (struct sockaddr *)&serverAddress, &serverAddressLen);
        printf("conected!!\n");
        //check if there is no exception
        if (serverSocket == -1) {
            printf("listen failed with error code : %d", errno);
            // close the sockets
            close(sock);
            return -1;
        }
        char bufferReply[BUFFER_SIZE];
        int bytesReceived = (int)(recv(serverSocket, bufferReply, BUFFER_SIZE, 0));
        if (bytesReceived == -1) {
            perror("Error: ");
        }
        printf("the password is: %s\n", bufferReply);
    }
    
}