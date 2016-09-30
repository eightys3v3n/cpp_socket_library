#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h> // include before windows.h
#include <windows.h>
#include <ws2tcpip.h>

struct Socket
{
  SOCKET s = INVALID_SOCKET;

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

  return 0;
}

int Socket::write(std::string m)
{
/*  int r = 0;

  r = ::write(fd, m.c_str(), m.length());
  if (r < 0)
    return r;

  return 0;*/
  return 1;
}

std::string Socket::read()
{
/*  char buf[4097];
  std::string r = "";
  int e = 0;

  while (true)
  {
    bzero(buf,4097);
    e = recv(fd,buf,4096,0);

    if (e == -1)
      break;

    r += std::string(buf);
  }

  return r;*/
  return "";
}

int Socket::close()
{
  closesocket(s);
  WSACleanup();

  return 0;
}

/* example code
int Socket::open()
{
    WSADATA wsaData;

    struct addrinfo *result = NULL, *ptr = NULL, hints;
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
    r = getaddrinfo(address, port, &hints, &result);
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
        printf("socket failed %ld\n", WSAGetLastError());
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
}

int Socket::close()
{
  closesocket(s);
  WSACleanup();
  return 0;
}*/