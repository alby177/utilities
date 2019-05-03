#include "TcpServer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <exception>
#include <thread>



#include <chrono>
#include <iostream>

TcpServer::TcpServer(int port, ErrorStruct *err, int maxClients)
{
	// Save user data
	mPort = port;
	mErr = err;
	mMaxClients = maxClients;
}

void TcpServer::CreateSocket()
{
	sockaddr_in servaddr;
  bool yes = true;

  // Create socket
  if ((mServerSock = socket( AF_INET, SOCK_STREAM, 0)) < 0)
  {
		// Check for error structure pointer
		if (mErr != nullptr)
		{
			*mErr << "Error creating socket";
			*mErr << errCodSettings;
		}
		throw std::exception();
  }

  // Set socket for binding already binded socket
  if(setsockopt(mServerSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
  {
		// Check for error structure pointer
		if (mErr != nullptr)
		{
			*mErr << "Error setting socket settings";
			*mErr << errCodSettings;
		}
		throw std::exception();
  }

  // Set memory structure for TCP/IP connection
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(mPort);

	// Bind created socket
	if (bind(mServerSock, reinterpret_cast<sockaddr *>(&servaddr), sizeof(servaddr)) < 0)
  {
		// Check for error structure pointer
		if (mErr != nullptr)
		{
			*mErr << "Error binding socket" ;
			*mErr << errCodSettings;
		}
		throw std::exception();
	}

	// Set socket to listen
	if (listen(mServerSock, mMaxClients) < 0)
  {
		// Check for error structure pointer
		if (mErr != nullptr)
		{
			*mErr << "Error listening from socket";
			*mErr << errCodSettings;
		}
		throw std::exception();
  }
}

void TcpServer::AddClientFunction(void (*clientFunction)(), void *clientData)
{
	// Run client function inside a thread
	std::thread t1(&TcpServer::RunClient, this, clientFunction, clientData);
	t1.detach();
}

void TcpServer::RunClient(void (*clientFunction)(), void *clientData)
{
	int clientSock, clientStructSize = 0;
	sockaddr_in cliaddr;

	while(true)
	{
		clientFunction();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		// // Check for maximum number of clients
		// if (mClientConnected < mMaxClients)
		// {
		// 	// Get client struct size
		// 	clientStructSize = sizeof(cliaddr);
		//
		// 	// Accept client connection
		// 	clientSock = accept(mServerSock, reinterpret_cast<sockaddr *>(&cliaddr), &clientStructSize);
		//
		//
		// }
	}
}


void TcpServer::Close()
{
	// Check for allocated server port
	if (mServerSock != 0)

		// Close server connection
		close(mServerSock);
}
