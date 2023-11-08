#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

const int PORT = 5350;
const int DATALEN = 3000;
const int BUFSIZE = 60000;
const int TIMEOUT_SEC = 0;
const int TIMEOUT_USEC = 100;
const std::string FILE_SEND = "file_send.txt";
const std::string FILE_RECEIVE = "file_receive.txt";

// data packet structure
struct pack_so {
  uint32_t num;        // the sequence number
  uint32_t len;        // the packet length
  char data[DATALEN];  // the packet data
};

struct ack_so {
  uint32_t num;
  uint8_t len;
};
