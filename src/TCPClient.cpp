#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>
#include <arpa/inet.h>

#include "TCPClient.h"
#include "exceptions.h"


/**********************************************************************************************
 * TCPClient (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 * //I don't totally understand this instruction... stdin is a file descriptor already isn't it?
 **********************************************************************************************/

TCPClient::TCPClient() {
    keyboard_fd = fileno(stdin);
    std::cout << "file descriptor for stdin is: " << keyboard_fd;
}

/**********************************************************************************************
 * TCPClient (destructor) - No cleanup right now
 *
 **********************************************************************************************/

TCPClient::~TCPClient() {

}

/**********************************************************************************************
 * connectTo - Opens a File Descriptor socket to the IP address and port given in the
 *             parameters using a TCP connection.
 *
 *    Throws: socket_error exception if failed. socket_error is a child class of runtime_error
 **********************************************************************************************/

void TCPClient::connectTo(const char *ip_addr, unsigned short port) {
    c_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(c_socket < 0){
        throw(socket_error("failed to create socket"));
    }
    serv_addr.sin_family = AF_INET;
    inet_aton(ip_addr, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);
    if( connect(c_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        throw(socket_error("failed to connect to server"));
    }
    connected = true;
    FD_SET(c_socket, &read_fds);

}

/**********************************************************************************************
 * handleConnection - Performs a loop that checks if the connection is still open, then 
 *                    looks for user input and sends it if available. Finally, looks for data
 *                    on the socket and sends it.
 * 
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::handleConnection() {
    int bytes_read = 0;
    std::cout << "Handling connection\n";
    while(connected){
        sleep(1);
        FD_ZERO(&read_fds);
        FD_SET(c_socket, &read_fds);
        FD_SET(fileno(stdin), &read_fds);
        if(select(c_socket+1, &read_fds, NULL, NULL, &timeout) < 0){
            throw (std::runtime_error("failure on client-select"));
        }
        std::cout << "test: " << FD_ISSET(fileno(stdin), &read_fds) << "\n";
        for(int i = 0; i < c_socket+1; i++){
            //std::cout << "i:" << i << " isset:" << FD_ISSET(i, &read_fds) << "\n";
            if(FD_ISSET(i, &read_fds)&&(i!=fileno(stdin))){
                bytes_read = recv(c_socket, buffer, sizeof(buffer), 0);
                if(bytes_read < 0){
                    throw(socket_error("failed reading from server"));
                }
                //std::cout <<"i is:" << i <<  "Bytes read is: " << bytes_read << ":len" << sizeof(buffer) << "\n";
                //if(bytes_read)
                buffer[bytes_read] = '\0';
                std::cout << buffer;
            } else if(FD_ISSET(fileno(stdin), &read_fds)){
                bytes_read = recv(fileno(stdin), buffer, sizeof(buffer), 0);
                std::cout << "in the read buffer" << buffer << "and" << bytes_read << "\n";
                if(bytes_read < 0){
                    throw(socket_error("failed reading stdin socket"));
                }
                bytes_read = send(c_socket, buffer, bytes_read, 0);
                if(bytes_read < 0){
                    throw(socket_error("failed to forward stdin to server"));
                }
                std::cout << "after the send buffer" << buffer << "and" << bytes_read << "\n";
            }

        }
    //     std::cout << "CMD:";
    //     std::cin >> 
    }
   
}

/**********************************************************************************************
 * closeConnection - Your comments here
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::closeConn() {
}


