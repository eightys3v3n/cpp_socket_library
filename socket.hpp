#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "platform.hpp" // Detect the platform if it wasn't specified
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#ifdef WINDOWS
  #include <winsock2.h> /** include before windows.h
                            otherwise something used to break.
                            hasn't been tested recently
                          */
  #include <windows.h> // see above
  #include <ws2tcpip.h>
#elif defined LINUX
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <string.h> // bzero()
  #include <netdb.h> // gethostbyname(), bcopy((char *)server->h_addr...
  #include <fcntl.h>
  #include <stdio.h>
#endif


#define BUFFER_SIZE 4096


struct PlatformDependant
{
#ifdef WINDOWS
  SOCKET socket       = INVALID_SOCKET;
  unsigned long mode  = 1;
  struct addrinfo addr;

  int get_addr_info(std::string host, unsigned short port, struct addrinfo &ret);
#elif defined LINUX
  int fd = 0;
  struct sockaddr address;

  int set_blocking();
  int set_nonblocking();
  int get_host_by_name(std::string hostname, unsigned short port, struct sockaddr &ret);
#endif
};

class Socket
{
private:
  std::string __host = "";
  unsigned short __port = 0;

public:
  const std::string host;
  const unsigned short port;
  PlatformDependant platform;

  Socket(std::string host, unsigned short port):
    __host(host), __port(port), // initialize __host as the host argument. same with port.
    host(__host), port(__port) {}; // Socket.host is a read-only version of __host. same with port.
  int connect();
  int send(std::string m);
  std::string receive(unsigned long size);
  std::string read(unsigned long size = 0);
  int close();
};

#endif // SOCKET_HPP