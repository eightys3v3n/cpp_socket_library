#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h> // include before windows.h
#include <windows.h>
#include <ws2tcpip.h>

struct Socket
{
  SOCKET s = INVALID_SOCKET;
  long unsigned* mode = new long unsigned(1);

  int open(std::string address, unsigned int port);
  int write(std::string m);
  std::string read();
  int close();
};

int Socket::open(std::string address, unsigned int port)
{
  WSADATA wsaData;
  struct addrinfo *result = NULL;
  struct addrinfo *ptr = NULL;
  struct addrinfo hints;
  int r;

  // Initialize Winsock
  r = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (r != 0)
  {
    printf("WSAStartup failed %d\n", r);
    return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  r = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result);
  if ( r != 0 )
  {
    printf("getaddrinfo failed %d\n", r);
    WSACleanup();
    return 1;
  }

  // Attempt to connect to an address until one succeeds
  for(ptr = result; ptr != NULL ;ptr = ptr->ai_next)
  {
    // Create a SOCKET for connecting to server
    s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (s == INVALID_SOCKET)
    {
      printf("socket failed %d\n", WSAGetLastError());
      WSACleanup();
      return 1;
    }

    // Connect to server.
    r = connect(s, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (r == SOCKET_ERROR)
    {
      closesocket(s);
      s = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);

  if (s == INVALID_SOCKET)
  {
    printf("unable to connect to server\n");
    WSACleanup();
    return 1;
  }

  ioctlsocket(s,FIONBIO,mode);

  return 0;
}

int Socket::write(std::string m)
{
  int r;
  r = send(s,m.c_str(),m.length(),0);
  return r;
}

std::string Socket::read()
{
  std::string r = "";
  int e = 0;

  while (true)
  {
    char buf[4096] = {};
    e = recv(s,buf,4096,0);

    if (e == SOCKET_ERROR)
      break;

    r += std::string(buf);
  }
  r += (char)0;

  return r;
}

int Socket::close()
{
  closesocket(s);
  WSACleanup();

  return 0;
}