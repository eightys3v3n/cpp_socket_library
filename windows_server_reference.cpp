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

// used for initialize()
WSADATA wsaData;

int result;

// the socket that the server is listening on. this is only used to accept new clients.
SOCKET listenSocket = INVALID_SOCKET;

// this is assigned when a client connects to the listenSocket. use this to talk to the client.
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
    return true;
  }

  return false;
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
 // receive until the peer shuts down the connection
  do
  {
    // read what the client sent us, result is the number of bytes that were read if it's > 0.
    result = recv(clientSocket, // the socket that is connected to the client.
      buffer, // the character array to store the received stuff in.
      BUFFER_LENGTH, // the number of characters to read.
      0); // no idea what this is for.

    if (result > 0)
    {
      std::cout << "received '" << std::string( buffer ) << "'" << std::endl;
    }
    else if (result == 0)
      std::cout << "received nothing, closing" << std::endl;

    // some windows error code stuff
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
  // i'm not sure what this is for.
  initialize();

  // opens a socket and starts listening on 0.0.0.0:PORT
  listen();

  // waits for a client to connect, then creates a socket with that client called clientSocket.
  accept();

  // read everything the client has sent.
  receive();

  // send a message back to the client.
  send();

  // related to the initialize function.
  WSACleanup();
  return 0;
}