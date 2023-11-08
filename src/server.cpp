#include "headsock.h"

double error_prob;

bool error() { return rand() < RAND_MAX * error_prob; }

void receive_file(int udpfd, double error_prob) {
  char buf[BUFSIZE];
  pack_so pack;
  ack_so ack;
  bool eof = false;
  int n = 0, i = 0, tot_bytes = 0;
  FILE *fp = fopen(FILE_RECEIVE.c_str(), "wb");
  sockaddr_in client_addr;
  printf("Receiving data via UDP...\n");
  while (!eof) {
    socklen_t addr_len = sizeof(sockaddr_in);
    n = recvfrom(udpfd, &pack, sizeof(pack_so), 0, (sockaddr *)&client_addr,
                 &addr_len);
    if (n == -1 || error()) {
      continue;
    }
    if (pack.num == -1) {
      eof = true;
      n = pack.len;
    }
    memcpy(buf, pack.data, n);
    ack.num = i;
    ack.len = pack.len;
    sendto(udpfd, &ack, sizeof(ack_so), 0, (sockaddr *)&client_addr, addr_len);
    i++;
    tot_bytes += pack.len;
    fwrite(buf, 1, pack.len, fp);
    buf[n] = '\0';
  }
  fclose(fp);
  printf("A file has been successfully received via UDP!\n");
  printf("The total data received is %d bytes.\n", (int)tot_bytes);
}

bool verify_file() {
  return system(("diff " + FILE_SEND + " " + FILE_RECEIVE).c_str());
}

int main(int argc, char *argv[]) {
  error_prob = argc >= 2 ? atof(argv[1]) : 0;
  int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udpfd < 0) {
    printf("Error in socket creation!");
    return 1;
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(addr.sin_zero), 8);
  bind(udpfd, (sockaddr *)&addr, sizeof(sockaddr));
  receive_file(udpfd, error_prob);
  if (!verify_file()) {
    printf("The file is received completely and correctly!\n");
  } else {
    printf("The received file failed to pass verification!\n");
  }
  close(udpfd);
}
