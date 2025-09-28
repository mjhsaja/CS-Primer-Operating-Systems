#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {
  int checksum, max, sock, conn, n;
  unsigned short port = 9876;
  struct sockaddr_in self, client;
  unsigned int clientlen;

  srand(0x1234);
  max = SIZE / sizeof(int);

  // construct the server socket, bind and listen for connections
  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&self, 0, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_addr.s_addr = htonl(INADDR_ANY);
  self.sin_port = htons(port);
  bind(sock, (struct sockaddr *)&self, sizeof(self));
  listen(sock, 10);

  // accept a new connection and stream it a bunch of random integers
  for (;;) {
    conn = accept(sock, (struct sockaddr *)&client, &clientlen);
    printf("Connection accepted\n");
    checksum = 0;
    for (int i = 0; i < max; i++) {
      n = rand();
      checksum ^= n;
      send(conn, &n, 4, 0);
    }
    printf("Sent %d random ints to client, checksum %d\n", max, checksum);
  }
}
