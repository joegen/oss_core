
#include "OSS/Net/Carp.h"

void usage()
{
  std::cout << std::endl << "Usage:  oss_garp INTERFACE IPADDR";
  std::cout << std::endl << "Example:  oss_garp eth0 192.168.1.10";
  std::cout << std::endl;
}

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    usage();
    exit(-1);
  }

  if (!OSS::Net::Carp::sendGratuitousArp(argv[1], argv[2]))
    exit(-1);

  return 0;
}
