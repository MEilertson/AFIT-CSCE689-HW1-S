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

#include "TCPServer.h"
#include "exceptions.h"

TCPServer::TCPServer() {

}


TCPServer::~TCPServer() {

}

/**********************************************************************************************
 * bindSvr - Creates a network socket and sets it nonblocking so we can loop through looking for
 *           data. Then binds it to the ip address and port
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::bindSvr(const char *ip_addr, short unsigned int port) {

    int opt = 1;

    s_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(  s_socketfd < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(s_socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
        perror("failed to set reuse addresses");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    inet_aton(ip_addr, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);

    if(bind(s_socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ){
        perror("Failed on bind");
        exit(EXIT_FAILURE);
    }
    FD_SET(s_socketfd, &master_fds);
    max_fd = s_socketfd;
   
}

std::string TCPServer::displayMenuString(){
       std::stringstream cmd_menu;
        cmd_menu << "**********WELCOME**********\n";
        cmd_menu << "-hello- (display greeting message)\n";
        cmd_menu << "-1-\n";
        cmd_menu << "-2-\n";
        cmd_menu << "-3-\n";
        cmd_menu << "-4-\n";
        cmd_menu << "-5-\n";
        cmd_menu << "-passwd-\n";
        cmd_menu << "-exit-\n";
        cmd_menu << "-menu-\n";
        cmd_menu << "****************************\n";

        return cmd_menu.str();
}

int TCPServer::sendMessage(const char* msg, int fd){
    std::cout << "sending message\n";
    ssize_t data_written = 0;
    while(data_written < strlen(msg)){
        ssize_t chunk = send(fd, msg + data_written, strlen(msg)- data_written, 0);
        std::cout << "writing data:" << data_written << "\n" << "Chunk is:" << chunk << "\n";
        std::cout << "size of:" << strlen(msg) << "\n";
        if(chunk == -1){
            throw(socket_error("failed to send message"));
        }
        std::cout << "after throw\n";
        data_written += chunk;
    }
    return data_written;
}

/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data. 
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::listenSvr() {
    if (listen(s_socketfd, 5) < 0){
        throw( std::runtime_error("Failure on listen"));
    }
    while(true){

        FD_ZERO(&read_fds);
        read_fds = master_fds;
        int test = select(max_fd+1, &read_fds, NULL, NULL, &timeout);
        if(test < 0){
            throw (std::runtime_error("failure on select"));
        }
        if(FD_ISSET(s_socketfd, &read_fds)){
            clilen = sizeof(client_addr);
            int new_fd = accept(s_socketfd, (struct sockaddr *) &client_addr, &clilen);
            if(new_fd < 0){
                throw( std::runtime_error("Failure on accepting new connection"));
            }
            //save new client information
            FD_SET(new_fd, &master_fds);
            //Display commands
            std::string menu = displayMenuString();
            sendMessage(menu.c_str(), new_fd);
        }
        for(int i = 0; i < max_fd; i++){
            if(FD_ISSET(i, &read_fds)&&(i!=s_socketfd)){
                std::cout << "should not be here for:" << s_socketfd << "\n";
                int byte_count = recv(i, s_buffer, sizeof(s_buffer), 0);
                if(byte_count < 0){
                    perror("error from socket");
                    throw(std::runtime_error("Failure reading from socket"));
                }
                else if(byte_count == 0){
                    std::cout << "danger.. \n";
                    //client closed connection
                    close(i);
                    FD_CLR(i, &master_fds);
                }
                else{
                    //handle data from client
                    std::cout << "From client:" << s_buffer;
                }
            }
        }


    }

}

/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::shutdown() {
}
