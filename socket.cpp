#ifdef WINDOWS
#include "socket_windows.cpp"
#else
#include "socket_unix.cpp"
#endif // PLATFORM

#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <stdio.h>

namespace tests
{

extern std::map<std::string, bool> status;

int openSocket(Socket& s)
{
  int r = 0;

  for (unsigned int c = 0; c < 5; c++)
  {
    if ( ! (r = s.open("127.0.0.1", 25555)))
      return 0;
  }

  printf("failed to open a connection to 127.0.0.1:25555:%i\n", r);
  return 1;
}

int writeSocket(Socket& s)
{
  s.write("hello there\n");

  printf("\"hello there\\n\" should have appeared in ncat, hit enter to complete the test\n");
  std::cin.ignore(255, '\n');

  return 0;
}

int readSocket(Socket& s)
{
  std::string buf = "";

  printf("waiting for \"hello there\"\n");

  for (unsigned int a = 0; a < 5; a++)
  {
    buf += s.read();
    if (buf.find("hello there\n") < buf.length())
    {
      printf("found \"hello there\"\n");
      return 0;
    }

    std::this_thread::sleep_for( std::chrono::seconds(2) );
  }

  printf("failed to read 'hello there\\n' from the socket:\"%s\"\n", buf.c_str());
  return 1;
}

int closeSocket(Socket& s)
{
  s.close();
  return 0;
}

int socket()
{
  Socket s;
  int r = 0;
  std::string b = "";

  printf("please start ncat listening on 127.0.0.1:25555 (ncat -l -k -p 25555)\n");
  std::cin.ignore(255, '\n');

  if (( r = openSocket(s) ))
  {
    printf("openSocket():%i\n", r);
    return 1;
  }

  if (( r = writeSocket(s) ))
  {
    printf("writeSocket():%i\n", r);
    return 2;
  }

  printf("please type 'hello there', without quotes, into netcat and hit enter\n");
  std::cin.ignore(255, '\n');

  if (( b = readSocket(s) ) == "")
  {
    printf("readSocket():\"%s\"\n", b.c_str());
    return 3;
  }

  if (( r = closeSocket(s) ))
  {
    printf("closeSocket():%i\n", r);
    return 4;
  }

  return 0;
}

}

#ifdef SA
int main()
{
  return tests::socket();
}
#endif // SA