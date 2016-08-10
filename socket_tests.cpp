#include <iostream>
#include <thread>
#include <chrono>
#include "socket.hpp"

namespace tests
{
  int openSocket(Socket& s)
  {
    int r = 0;

    for (unsigned int c = 0; c < 5; c++)
    {
      if ( ! (r = s.open("127.0.0.1", 25555)))
        return 0;
    }

    std::cout << "failed to open a connection to 127.0.0.1:25555:" << r << std::endl;
    return 1;
  }

  int writeSocket(Socket& s)
  {
    s.write("hello there\n");

    std::cerr << "'hello there\\n' should have appeared in ncat, hit enter to complete the test.";
    std::cin.ignore(255, '\n');

    return 0;
  }

  int readSocket(Socket& s)
  {
    std::string buf = "";

    std::cout << "waiting for 'hello there'" << std::endl;

    for (unsigned int a = 0; a < 5; a++)
    {
      buf += s.read();
      if (buf.find("hello there\n") < buf.length())
      {
        std::cout << "found 'hello there\n'." << std::endl;
        return 0;
      }

      std::this_thread::sleep_for( std::chrono::seconds(2) );
    }

    std::cout << "failed to read 'hello there\\n' from the socket:'" << buf << "'" << std::endl;
    return 1;
  }

  int socket()
  {
    Socket s;
    int r = 0;
    std::string b = "";

    std::cerr << "please start ncat listening on 127.0.0.1:25555 (ncat -l -k -p 25555)";
    std::cin.ignore(255, '\n');

    if (( r = openSocket(s) ))
    {
      std::cout << "openSocket():" << r << std::endl;
      return 1;
    }

    if (( r = writeSocket(s) ))
    {
      std::cout << "writeSocket():" << r << std::endl;
      return 1;
    }

    std::cerr << "please type 'hello there', without quotes, into netcat and hit enter.";
    std::cin.ignore(255, '\n');

    if (( b = readSocket(s) ) == "")
    {
      std::cout << "readSocket():'" << b << "'" << std::endl;
      return 1;
    }

    return 0;
  }
}