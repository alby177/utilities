#include "TcpServer.h"
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <exception>
#include <thread>
#include <vector>

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

void TcpServer::AddClientFunction(void (*clientFunction)(ServerStruct *serverStruct), void *clientData)
{
	// Set server running flag
	mServerRunning = true;

	// Run client function inside a thread
	std::thread t1(&TcpServer::RunServer, this, clientFunction, clientData);
	t1.detach();
}

void TcpServer::RunServer(void (*clientFunction)(ServerStruct *serverStruct), void *clientData)
{
	int clientSock;
	socklen_t clientStructSize;
	sockaddr_in cliaddr;
	std::vector<std::thread> threadsVector;

	// Check for authorization to run
	while(mStopServer == false)
	{
		// Check for maximum number of clients
		if (mClientConnected < mMaxClients)
		{
			// Get client struct size
			clientStructSize = sizeof(cliaddr);

			// Accept client connection
			clientSock = accept(mServerSock, reinterpret_cast<sockaddr *>(&cliaddr), &clientStructSize);

			// Check for client connection error
			if( clientSock < 0)
			{
				*mErr << "Error accepting client connection";
			}
			else
			{
				// Populate server data struct
				ServerStruct *serverData = new ServerStruct;
				serverData->clientData = cliaddr;
				serverData->clientSock = clientSock;
				serverData->userData = clientData;

				// Create thread
				threadsVector.push_back(std::thread(&TcpServer::RunClient, this, clientFunction, serverData));

				// Lock access to common resource
				mLock.lock();

				// Increase connected thread number
				mClientConnected++;

				// Unlock access to common resource
				mLock.unlock();
			}
		}
	}

	// Synchronize threads
	for (auto &i: threadsVector)
		i.join();

	// Remove all threads
	threadsVector.clear();

	// Set server execution end flag
	mServerRunning = false;

	// Reset execution stop flags
	mStopServer = false;
}

void TcpServer::RunClient(void (*clientFunction)(ServerStruct *serverStruct), ServerStruct *serverData)
{
	// Run client function
	while(mStopServer == false)
		clientFunction(serverData);

	// Clean server data structure
	delete serverData;

	// Lock access to common resource
	mLock.lock();

	// Increase connected thread number
	mClientConnected--;

	// Unlock access to common resource
	mLock.unlock();
}

void TcpServer::StopServer()
{
	// Set execution stop flags
	mStopServer = true;
}

void TcpServer::WaitForServerEnd()
{
	// Wait server to finish execution
	while(mServerRunning == true);
}

void TcpServer::Close()
{
	// Check for allocated server port
	if (mServerSock != 0)

		// Close server connection
		close(mServerSock);
}

size_t TcpServer::Send(ServerStruct *serverStruct, const std::string &message)
{
	// Send message to client
	size_t byteSent = send(serverStruct->clientSock, message.c_str(), message.size(), 0);
	return byteSent;
}

size_t TcpServer::Receive(ServerStruct *serverStruct, std::string &message)
{
	char mess[500];

	// Receive message from client
	size_t received = recv(serverStruct->clientSock, mess, sizeof(mess), 0);

	// Save received data
	message = mess;

	// Save received data
	return received;
}
