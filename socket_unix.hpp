#ifndef SOCKET
#define SOCKET

#include <string>

struct Socket
{
  int fd;

  int open(std::string address, unsigned int port);
  int write(std::string m);
  std::string read();
};

#endif // SOCKET