#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <string>

enum ErrorCodes
{
  errCodCreate,               // Error creating socket
  errCodSettings,             // Error setting socket options
  errCodBind,                 // Error binding socket
  errCodListen,               // Error putting socket on listening
};

struct ErrorStruct
{
private:
  ErrorCodes errCode;
  std::string errString;

public:
  //std::ostream &operator << (std::ostream &out) {out << errString << ". With error code: " << errCode; return out;}
  void operator << (const std::string in) {errString = in;}
  void operator << (const char *in) {errString = in;}
  void operator << (const ErrorCodes &in) {errCode = in;}
  void operator << (const int &in) {errCode = static_cast<ErrorCodes>(in);}
};

class TcpServer
{
public:
  TcpServer(int port, ErrorStruct *err = nullptr, int maxClients = 5);
  ~TcpServer(){}
  int AddClient(void *clientFunction, void *clientData = nullptr);  // Run connected client code
  const inline int GetPortNumber() {return mPort;}  // Get port number

private:
  // Private methods


  // private members
  int mPort = 0;            // Port number
  int mServerSock = 0;      // Server socket descriptor
  int mClientConnected = 0; // Number of conected clients
};

#endif
