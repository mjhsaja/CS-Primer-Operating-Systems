#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {
  int checksum, max, sock, n;
  unsigned short port = 9876;
  struct timespec start, end;
  struct sockaddr_in server;

  max = SIZE / sizeof(int);

  // construct the client socket, and connect
  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(port);
  connect(sock, (struct sockaddr *)&server, sizeof(server));

  // receive a bunch of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  checksum = 0;
  for (int i = 0; i < max; i++) {
    recv(sock, &n, 4, 0);
    checksum ^= n;
  }
  clock_gettime(CLOCK_MONOTONIC, &end);

  float secs =
      (float)(end.tv_nsec - start.tv_nsec) / 1e9 + (end.tv_sec - start.tv_sec);
  float mibs = (float)SIZE / secs / (1 << 20);

  printf("Received at %.3f MiB/s. Checksum: %d\n", mibs, checksum);
}
