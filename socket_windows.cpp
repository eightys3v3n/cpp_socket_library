#include "socket.hpp"


int PlatformDependant::get_addr_info(std::string host, unsigned short port)
{
  int result;
  struct addrinfo *addresses = nullptr;

  result = getaddrinfo(host.c_str(), std::to_string(port).c_str(), nullptr, &addresses);
  if (result == 0)
  {
    address = * addresses;
  }
  else
  {
    #if VERBOSITY >= 1
      printf("Failed to getaddrinfo: %d\n", result);
    #endif
  }

  return result;
}


int Socket::connect()
{
  WSADATA wsaData;
  int result;

  // Initialize Winsock
  result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (result != 0)
  {
    printf("WSAStartup failed %d\n", result);
    return 1;
  }

  // Resolve the server address and port
  result = platform.get_addr_info(__host, __port);
  if (result != 0)
  {
    printf("get_addr_info failed %d\n", result);
    freeaddrinfo(&platform.address);
    WSACleanup();
    return 1;
  }

  // Create a SOCKET for connecting to server
  platform.socket = ::socket(platform.address.ai_family, platform.address.ai_socktype, platform.address.ai_protocol);
  if (platform.socket == INVALID_SOCKET)
  {
    printf("Socket creation failed %d\n", WSAGetLastError());
    freeaddrinfo(&platform.address);
    WSACleanup();
    return 1;
  }

  // Connect to server.
  result = ::connect(platform.socket, platform.address.ai_addr, (int)platform.address.ai_addrlen);
  if (result == SOCKET_ERROR)
  {
    closesocket(platform.socket);
    freeaddrinfo(&platform.address);
    platform.socket = INVALID_SOCKET;
    return 1;
  }

  if (platform.socket == INVALID_SOCKET)
  {
    printf("unable to connect to server\n");
    WSACleanup();
    return 1;
  }

  return 0;
}


int Socket::send(std::string m)
{
  int result;
  result = ::send(platform.socket, m.c_str(), m.length(), 0);
  return result;
}


std::string Socket::receive(unsigned long size)
{
  char buff[BUFFER_SIZE+1];
  std::string received = "";
  int left_to_receive = size;
  int to_receive = BUFFER_SIZE;
  int result;

  while (left_to_receive > 0)
  {
    if (left_to_receive < BUFFER_SIZE)
    {
      to_receive = left_to_receive;
    }

    memset(buff, 0, BUFFER_SIZE+1);
    result = ::recv(platform.socket, buff, to_receive, 0);

    if (result == -1)
    {
      #if VERBOSITY >= 2
        printf("An error occured %d.\n", result);
      #endif
      break;
    }

    left_to_receive -= result;
    received += std::string(buff);
    #if VERBOSITY >= 3
      printf("Received %d bytes.\n", result);
    #endif

    std::this_thread::sleep_for(std::chrono::milliseconds(platform.BLOCKING_RECEIVE_DELAY));
  }

  #if VERBOSITY >= 2
    printf("Received '%s'.\n", received.c_str());
  #endif
  return received;
}


std::string Socket::read(unsigned long size)
{
  char buff[BUFFER_SIZE+1];
  std::string strbuff = "";
  std::string received = "";
  int left_to_receive;
  int to_receive = BUFFER_SIZE;
  int result;

  if (size == 0) // receive all available
  {
    left_to_receive = BUFFER_SIZE+1;
    #if VERBOSITY >= 2
      printf("Reading all received bytes...\n");
    #endif
  }
  else // receive only size bytes
  {
    left_to_receive = size;
    #if VERBOSITY >= 2
      printf("Reading %lu bytes...\n", size);
    #endif
  }

  while (left_to_receive > 0)
  {
    if (left_to_receive < BUFFER_SIZE)
    {
      to_receive = left_to_receive;
    }

    memset(buff, 0, BUFFER_SIZE+1);
    result = ::recv(platform.socket, buff, to_receive, 0);

    if (result == -1 and errno != EWOULDBLOCK)
    {
      #if VERBOSITY >= 2
        printf("An error occured %d.\n", errno);
      #endif
      break;
    }
    else if (result == -1 and errno == EWOULDBLOCK)
    {
      #if VERBOSITY >= 2
        printf("Reached end-of-stream; socket has been closed and all data received.\n");
      #endif
      left_to_receive = 0;
    }
    else if (size == 0)
    {
      left_to_receive = BUFFER_SIZE+1;
    }

    strbuff = std::string(buff);
    left_to_receive -= strbuff.length();
    received += strbuff;
    #if VERBOSITY >= 3
      printf("Received %lu bytes.\n", strbuff.length());
    #endif
  }

  #if VERBOSITY >= 2
    printf("Received '%s'.\n", received.c_str());
  #endif
  return received;
}

// std::string Socket::read(unsigned long size)
// {
//   std::string received = "";
//   int result = 0;

//   while (true)
//   {
//     char buf[4096] = {};
//     result = recv(platform.socket, buf, 4096, 0);
//     if (result == SOCKET_ERROR)
//       break;

//     received += std::string(buf);
//   }
//   received += (char)0;

//   return received;
// }

int Socket::close()
{
  closesocket(platform.socket);
  WSACleanup();

  return 0;
}