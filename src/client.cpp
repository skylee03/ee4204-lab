#include "headsock.h"

void send_file(int udpfd, sockaddr *server_addr) {
  using namespace std::chrono;
  char buf[BUFSIZE];
  int n = 0, i = 0;
  pack_so sends;
  ack_so acks;
  bool eof = false;
  int total_data_size = 0;
  printf("Transmitting data via UDP...\n");
  FILE *fp = fopen(FILE_SEND.c_str(), "r");
  auto start_time = high_resolution_clock::now();
  while (!eof) {
    n = fread(buf, 1, DATALEN, fp);
    for (;;) {
      sends.num = i;
      sends.len = n;
      memcpy(sends.data, buf, n);
      if (n < DATALEN) {
        eof = true;
        sends.num = -1;
      }
      sendto(udpfd, &sends, sizeof(pack_so), 0, server_addr, sizeof(sockaddr));
      struct timeval timeout;
      timeout.tv_sec = TIMEOUT_SEC;
      timeout.tv_usec = TIMEOUT_USEC;
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(udpfd, &readfds);
      if (select(udpfd + 1, &readfds, NULL, NULL, &timeout)) {
        recvfrom(udpfd, &acks, sizeof(ack_so), 0, NULL, NULL);
        i++;
        total_data_size += n;
        break;
      }
      printf("ACK not received. Retransmitting packet %d...\n", i);
    }
  }
  auto end_time = high_resolution_clock::now();
  fclose(fp);
  duration<double> total_transfer_time = end_time - start_time;
  double throughput = total_data_size / total_transfer_time.count() * 8 / 1e6;
  printf("File sent successfully!\n");
  printf("Total data size: %lf MB.\n", total_data_size / 1e6);
  printf("Transmission time: %lf s.\n", total_transfer_time.count());
  printf("Throughput: %lf Mbps\n", throughput);
}

int main() {
  hostent *host = gethostbyname("localhost");
  char *buf = (char *)malloc(sizeof(char) * DATALEN);
  int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr = *((in_addr *)host->h_addr);
  bzero(&(server_addr.sin_zero), 8);
  send_file(udpfd, (sockaddr *)&server_addr);
  close(udpfd);
  free(buf);
}
