#include "socket.hpp"


/**
    Set the socket to blocking mode; IE wait until you get what you want.

    Returns:
      The result from `fcntl(, F_SETFL,)`
  */
int PlatformDependant::set_blocking()
{
  int flags;
  int ret;

  flags = fcntl(fd, F_GETFL, 0);
  flags = flags & ~O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);

  #if VERBOSITY >= 2
  if (ret == 0)
    std::cout << "Set socket to blocking." << std::endl;
  else
    std::cerr << "Failed to set socket to blocking." << std::endl;
  #endif

  return ret;
}


/**
    Set the socket to non-blocking mode; IE just return what's available, don't wait.

    Returns:
      The result from `fcntl(, F_SETFL,)`
  */
int PlatformDependant::set_nonblocking()
{
  int flags;
  int ret;

  flags = fcntl(fd, F_GETFL, 0);
  flags = flags | O_NONBLOCK;
  ret = fcntl(fd, F_SETFL, flags);

  #if VERBOSITY >= 2
  if (ret == 0)
    std::cout << "Set socket to non-blocking." << std::endl;
  else
    std::cerr << "Failed to set socket to non-blocking." << std::endl;
  #endif

  return ret;
}


/**
    Takes the hostname as a string and the port as an unsigned short.
    Sets ret to point toward the generated sockaddr.
*/
int PlatformDependant::get_host_by_name(std::string hostname, unsigned short port, struct sockaddr &ret)
{
  struct sockaddr_in addr;
  struct hostent* server;

  server = gethostbyname(hostname.c_str());
  if (server != 0)
  {
    bzero((char*)&addr, sizeof(addr)); // initialize addr to zeros

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);
    ret = *(struct sockaddr*)&addr;
  }
  else
  {
    #if VERBOSITY >= 1
      printf("Failed to resolve server address.\n");
    #endif
    return 1;
  }

  return 0;
}


int Socket::connect()
{
  int result;

  platform.fd = socket(AF_INET, SOCK_STREAM, 0);
  if (platform.fd < 0)
  {
    #if VERBOSITY >= 1
      printf("failed to create a socket\n");
    #endif
    return 1;
  }

  result = platform.get_host_by_name(__host, __port, platform.address);

  result = ::connect(platform.fd, &platform.address, sizeof(platform.address));
  if (result < 0)
  {
    #if VERBOSITY >= 1
      printf("coundn't connect to \"%s:%d\"\n", __host.c_str(), __port);
    #endif
    return 2;
  }

  fcntl(platform.fd, F_SETFL, O_NONBLOCK);

  return 0;
}


int Socket::send(std::string m)
{
  int r = 0;

  r = ::write(platform.fd, m.c_str(), m.length());
  if (r < 0)
    return r;

  return 0;
}


std::string Socket::receive(unsigned long size)
{
  char buff[BUFFER_SIZE+1];
  std::string received = "";
  int left_to_receive = size;
  int to_receive = BUFFER_SIZE;
  int result = 0;

  result = platform.set_blocking();
  if (result != 0)
  {
    printf("Failed to set blocking mode.\n");
  }

  while (left_to_receive > 0)
  {
    if (left_to_receive < BUFFER_SIZE)
    {
      to_receive = left_to_receive;
    }

    bzero(buff, BUFFER_SIZE+1);
    result = ::recv(platform.fd, buff, to_receive, 0);

    if (result == -1)
    {
      #if VERBOSITY >= 2
        printf("An error occured %d.\n", result);
      #endif
      break;
    }
    else if (result == 0)
    {
      #if VERBOSITY >= 2
        printf("Reached end-of-stream; socket has been closed and all data received.\n");
      #endif
      left_to_receive = 0;
    }

    left_to_receive -= result;
    received += std::string(buff);
    #if VERBOSITY >= 3
      printf("Received %d bytes.\n", result);
    #endif
  }

  #if VERBOSITY >= 2
    printf("Received '%s'.\n", received.c_str());
  #endif
  return received;
}


std::string Socket::read(unsigned long size)
{
  char buff[BUFFER_SIZE+1];
  std::string received = "";
  int left_to_receive;
  int to_receive = BUFFER_SIZE;
  int result = 0;

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

  result = platform.set_nonblocking();
  if (result != 0)
  {
    printf("Failed to set non-blocking mode.\n");
  }


  while (left_to_receive > 0)
  {
    if (left_to_receive < BUFFER_SIZE)
    {
      to_receive = left_to_receive;
    }

    bzero(buff, BUFFER_SIZE+1);
    result = ::recv(platform.fd, buff, to_receive, 0);

    if (result == -1)
    {
      #if VERBOSITY >= 2
        printf("An error occured %d.\n", result);
      #endif
      break;
    }
    else if (result == 0)
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

    left_to_receive -= result;
    received += std::string(buff);
    #if VERBOSITY >= 3
      printf("Received %d bytes.\n", result);
    #endif
  }

  #if VERBOSITY >= 2
    printf("Received '%s'.\n", received.c_str());
  #endif
  return received;
}


int Socket::close()
{
  ::close(platform.fd);

  return 0;
}