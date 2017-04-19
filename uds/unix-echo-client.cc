#include "unix-client.h"

int
main(int argc, char **argv)
{
  int num = 40;
  if(argc > 1) num = atoi(argv[1]);
    UnixClient client = UnixClient(num);
    client.run();
}

