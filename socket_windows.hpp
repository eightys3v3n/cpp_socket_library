struct Socket
{
  int open(std::string address, unsigned int port);
  int write(std::string m);
  std::string read();
};