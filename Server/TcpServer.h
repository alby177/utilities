#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <string>
#include <mutex>
#include <arpa/inet.h>

enum ErrorCodes
{
  errCodCreate = 1000,        // Error creating socket
  errCodSettings,             // Error setting socket options
  errCodBind,                 // Error binding socket
  errCodListen,               // Error putting socket on listening
};

struct ErrorStruct
{
public:
  friend std::ostream &operator << (std::ostream &out, ErrorStruct &err)
    {out << err.errString + ". Error code: " + std::to_string(static_cast<int>(err.errCode)); return out;}
  void operator << (const std::string in) {errString = in;}
  void operator << (const char *in) {errString = in;}
  void operator << (const ErrorCodes &in) {errCode = in;}
  void operator << (const int &in) {errCode = static_cast<ErrorCodes>(in);}

private:
  ErrorCodes errCode;
  std::string errString;
};

struct ServerStruct
{
public:
  sockaddr_in         clientData;                    // IP client data
  int                 clientSock;                    // Client socket address
  void const          *userData;                      // Data provided by the user
};

class TcpServer
{
public:
  TcpServer(int port = 2000, ErrorStruct *err = nullptr, int maxClients = 5);
  ~TcpServer(){}
  void CreateSocket();
  void AddClientFunction(void (*clientFunction)(ServerStruct *serverStruct), void *clientData = nullptr);  // Run connected client code
  const inline int GetPortNumber() {return mPort;}  // Get port number
  void StopServer();
  void WaitForServerEnd();
  void Close();

  // Static funtion declaration
  static size_t Send(ServerStruct *serverStruct, const std::string &message);
  static size_t Receive(ServerStruct *serverStruct, std::string &message);

private:
  // Private methods
  void RunServer(void (*clientFunction)(ServerStruct *serverStruct), void *clientData = nullptr);
  void RunClient(void (*clientFunction)(ServerStruct *serverStruct), ServerStruct *serverData = nullptr);

  // private members
  int               mPort               = 0;            // Port number
  int               mServerSock         = 0;            // Server socket descriptor
  int               mMaxClients         = 0;            // Maximum number of connectable clients
  int               mClientConnected    = 0;            // Number of conected clients
  bool              mServerRunning      = false;        // Flag indicating if the server is running
  bool              mStopServer         = false;        // Flag for server exection termination
  std::mutex        mLock;                              // Mutex for preventing acces to variable containing the number of clients running
  ErrorStruct       *mErr               = nullptr;      // Occurred error saving structure
};

#endif
