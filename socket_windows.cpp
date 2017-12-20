#include "socket.hpp"


int PlatformDependant::get_addr_info(std::string host, unsigned short port, struct addrinfo &ret)
{
  int result;
  struct addrinfo *addresses = nullptr;

  result = getaddrinfo(host.c_str(), std::to_string(port).c_str(), nullptr, &addresses);
  if (result != 0)
  {
    #if VERBOSITY >= 1
    printf("Failed to getaddrinfo: %d\n", result);
    #endif
    return result;
  }

  ret = *addresses;

  return 0;
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
  result = get_addr_info(__host, __port, platform.address);
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
    continue;
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

std::string Socket::read()
{
  std::string received = "";
  int result = 0;

  while (true)
  {
    char buf[4096] = {};
    result = recv(platform.socket, buf, 4096, 0);
    if (result == SOCKET_ERROR)
      break;

    received += std::string(buf);
  }
  received += (char)0;

  return received;
}

int Socket::close()
{
  closesocket(platform.socket);
  WSACleanup();

  return 0;
}