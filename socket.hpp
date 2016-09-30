#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#ifdef WINDOWS
#include <windows.h>
#endif // PLATFORM

struct Socket
{
	#ifdef WINDOWS
	SOCKET s = INVALID_SOCKET;
	long unsigned* mode;
	#else
	int fd;
	#endif // PLATFORM

  int open(std::string address, unsigned int port);
  int write(std::string m);
  std::string read();
  int close();
};

namespace tests
{
  int socket();
}

#endif // SOCKET_HPP