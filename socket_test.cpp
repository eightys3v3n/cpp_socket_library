#include <iostream>
#include "socket_tests.hpp"

int main()
{
  if (tests::socket())
    return 1;

  return 0;
}