#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Server.h"

class TCPServer : public Server 
{
public:
   TCPServer();
   ~TCPServer();

   void bindSvr(const char *ip_addr, unsigned short port);
   void listenSvr();
   std::string displayMenuString();
   int sendMessage(const char* msg, int fd);
   void shutdown();

private:
   int s_socketfd;
   int max_fd;

   socklen_t clilen;

   struct sockaddr_in serv_addr;
   struct sockaddr_in client_addr;
   struct timeval timeout = {1, 0};

   char s_buffer[512];

   fd_set master_fds;
   fd_set read_fds;
   fd_set write_fds;

};


#endif
