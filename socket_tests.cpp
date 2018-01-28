#include <cstdio>
#include <cassert>
#include "socket.hpp"


bool test_client()
{
  std::string buff = "";
  int result;

  Socket s("127.0.0.1", 25555);
  printf("Initialized socket. Please start a server on %s:%d and press enter.",
    s.host.c_str(), s.port);
  getchar();

  printf("Attempting to connect...\n");
  result = s.connect();
  if (result)
  {
    printf("Failed to connect %d.\n", result);
    return true;
  }
  printf("Connected.\n");

  buff = "hello world\n";
  s.send(buff);
  printf("Sent '%s'.\n", buff.c_str());
  getchar();

  printf("Please send 7 or more bytes all at once.\n");
  printf("Waiting to receive 4 bytes...\n");
  buff = s.receive(4);
  assert(buff.length() == 4);
  printf("Received '%s'.\n", buff.c_str());

  printf("Reading 2 more bytes...\n");
  buff += s.read(2);
  assert(buff.length() == 6);
  printf("Received '%s'.\n", buff.substr(4).c_str());

  printf("Reading the rest of received bytes...\n");
  buff += s.read();
  assert(buff.length() > 6);
  printf("Received '%s'.\n", buff.substr(6).c_str());

  s.close();
  printf("Closed socket.\n");

  return false;
}


// bool test_server() {}


int main()
{
  if (test_client())
    return 1;
}
