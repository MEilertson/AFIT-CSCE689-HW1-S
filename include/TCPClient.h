#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Client.h"

// The amount to read in before we send a packet
const unsigned int stdin_bufsize = 50;
const unsigned int socket_bufsize = 100;

class TCPClient : public Client
{
public:
   TCPClient();
   ~TCPClient();

   virtual void connectTo(const char *ip_addr, unsigned short port);
   virtual void handleConnection();
   

   virtual void closeConn();

private:
   char buffer[256];
   int keyboard_fd;
   int c_socket;
   bool connected;
   struct sockaddr_in serv_addr;
   struct hostent *h;
   struct timeval timeout = {1, 0};

   fd_set read_fds;
   fd_set write_fds;
   

};


#endif
