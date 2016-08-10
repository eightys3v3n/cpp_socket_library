#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define BUFFER_LENGTH 512
#define PORT "27015"
#pragma comment (lib, "Ws2_32.lib")

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

WSADATA wsaData;
int result;
SOCKET listenSocket = INVALID_SOCKET;
SOCKET clientSocket = INVALID_SOCKET;
struct addrinfo *addressInfo = 0;
struct addrinfo hints;
char buffer[ BUFFER_LENGTH ];

bool initialize()
{
  result = WSAStartup( MAKEWORD(2,2), &wsaData );

  if ( result != 0 )
  {
    std::cout << "WSAStartup failed with error: " << result << std::endl;
    return 1;
  }
}

bool listen()
{
  // useless shit?
  ZeroMemory( &hints, sizeof( hints ) );
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // resolve hosts ip even if i want it to be 0.0.0.0?
  // what the actual fuck windows.
  result = getaddrinfo( 0, PORT, &hints, &addressInfo );

  if ( result != 0 )
  {
    printf("getaddrinfo failed with error: %d\n", result);
    return 1;
  }

  /* create a socket to listen with
   socket(addressType, socketType, protocol)
   really only (AF_INET, SOCK_STREAM, IPPROTO_TCP)
   or (ipv4, socket iostream, tcp)
  */
  listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSocket == INVALID_SOCKET)
  {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    freeaddrinfo(addressInfo);
    return 1;
  }

  /* bind the port
  the ai_addr is a pointer to a stupid data type i don't want to mimic so it can stay the way it is.
  */
  result = bind( listenSocket, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);
  if (result == SOCKET_ERROR)
  {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(addressInfo);
    closesocket(listenSocket);
    return 1;
  }

  // free address info, but you don't have to?
  freeaddrinfo(addressInfo);

  /* start listening finally.
  listen(socket, maxConnectionsInQueue)
  SOMAXCONN means "windows you choose, i don't give a shit"
  */
  result = listen(listenSocket, SOMAXCONN);
  if (result == SOCKET_ERROR)
  {
    printf("listen failed with error: %d\n", WSAGetLastError());
    closesocket(listenSocket);
    return 1;
  }
}

bool accept()
{
  /* accept a client connection into clientSocket
  accept( listeningSocket, address, addressLength)
  those extra two variables could be used to accept a specific client.
  */
  clientSocket = accept( listenSocket, 0, 0 );
  if (clientSocket == INVALID_SOCKET)
  {
      printf("accept failed with error: %d\n", WSAGetLastError());
      closesocket(listenSocket);
      return 1;
  }
}

bool closeListener()
{
  // close the listener
  closesocket(listenSocket);
}

bool closeConnection()
{
  /* shutdown the client connection
  shutdown( socket, type )
  SD_SEND - stop all sending stuffs and wait to shutdown
  SD_RECEIVE - stop receiving and shutdown
  SD_BOTH - stop everything and shutdown
  */
  result = shutdown(clientSocket, SD_SEND);
  if (result == SOCKET_ERROR)
  {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(clientSocket);
    return 1;
  }

  // cleanup
  closesocket(clientSocket);
}

bool receive()
{
 // Receive until the peer shuts down the connection
  do
  {
    // listen to the other guy's shrill screams.
    result = recv(clientSocket, buffer, BUFFER_LENGTH, 0);

    if (result > 0)
    {
      std::cout << "received '" << std::string( buffer ) << "'" << std::endl;
    }
    else if (result == 0)
      std::cout << "received nothing, closing" << std::endl;

    // some windows error code shit
    else
    {
      printf("recv failed with error: %d\n", WSAGetLastError());
      closesocket(clientSocket);
      return 1;
    }
  } while (result > 0);
}

bool send()
{
  // yell some stuff back at said crazy guy.
  result = send( clientSocket, buffer, result, 0 );
  if (result == SOCKET_ERROR)
  {
    printf("send failed with error: %d\n", WSAGetLastError());
    closesocket(clientSocket);
    return 1;
  }

  std::cout << "sent '" << buffer << "'" << std::endl;
}

int main()
{
  initialize();
  listen();
  accept();
  receive();
  send();

  WSACleanup();
  return 0;
}