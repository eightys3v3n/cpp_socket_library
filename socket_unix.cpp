#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // bzero()
#include <netdb.h> // gethostbyname(), bcopy((char *)server->h_addr...
#include <fcntl.h>
#include <stdio.h>

struct Socket
{
  int fd = 0;

  int open(std::string address, unsigned int port);
  int write(std::string m);
  std::string read();
  int close();
};

int Socket::open(std::string address, unsigned int port)
{
  struct sockaddr_in addr;
  struct hostent *server;

  fd = socket(AF_INET,SOCK_STREAM, 0);
  if (fd < 0)
  {
    #ifdef VERBOSE
    printf("failed to create a socket\n");
    #endif // VERBOSE
    return 1;
  }

  // depreciated | gethostbyname()
  server = gethostbyname(address.c_str());
  if (server == 0)
  {
    #ifdef VERBOSE
    printf("host not found \"%s\"\n", address.c_str());
    #endif // VERBOSE
    return 2;
  }

  bzero((char*)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
    #ifdef VERBOSE
    printf("coundn't connect to \"%s:%o\"\n", address, port);
    #endif // VERBOSE
    return 3;
  }

  fcntl(fd, F_SETFL, O_NONBLOCK);

  return 0;
}

int Socket::write(std::string m)
{
  int r = 0;

  r = ::write(fd, m.c_str(), m.length());
  if (r < 0)
    return r;

  return 0;
}

std::string Socket::read()
{
  char buf[4097];
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

  return r;
}

int Socket::close()
{
  close(fd);

  return 0;
}