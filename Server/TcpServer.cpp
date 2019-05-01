#include "TcpServer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

TcpServer::TcpServer(int port, ErrorStruct *err, int maxClients)
{
	struct sockaddr_in servaddr;
  bool yes = true;

  // Save port number
  mPort = port;

  // Create socket
  if ((mServerSock = socket( AF_INET, SOCK_STREAM, 0)) < 0)
  {
    *err << "Error creating socket";
    *err << errCodCreate;
  }

  // Set socket for binding already binded socket
  if(setsockopt(SOL_SOCKET, SO_REUSEADDR, SO_KEEPALIVE, &yes, sizeof(int)) < 0)
  {
    *err << "Error setting socket settings";
    *err << errCodSettings;
  }

  // Set memory structure for TCP/IP connection
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(mPort);

	// Bind created socket
	if (bind(mServerSock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
  {
    *err << "Error binding socket" ;
    *err << errCodBind;
	}

	// Set socket to listen
	if (listen(mServerSock, maxClients) < 0)
  {
    *err << "Error listening from socket";
    *err << errCodListen;
  }
}
