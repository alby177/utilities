#ifndef TCPSERVER_H
#define TCPSERVER_H

class TcpServer
{
public:
  TcpServer();
  ~TcpServer();
  int Connect(int port = 0);
  int AddClient();

private:
  // Private methods
  

  // private members
  int mPort = 0;
}

#endif
