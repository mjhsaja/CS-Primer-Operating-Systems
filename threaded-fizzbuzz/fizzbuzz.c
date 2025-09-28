#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int n = 0;

void *fizz(void *arg) {
  while (1) {
    if (n % 3 == 0) {
      n = 1;
      printf("\tfizz");
    }
  }
}

void *buzz(void *arg) {
  while (1) {
    if (n % 5 == 0) {
      n = 1;
      printf("\tbuzz");
    }
  }
}

int main () {
  // start two threads, running fizz and buzz respectively
  pthread_t t1, t2;
  pthread_create(&t1, NULL, fizz, NULL);
  pthread_create(&t2, NULL, buzz, NULL);
  // every 100ms, update n randomly from the range [0, 16), indefinitely
  while (1) {
    n = rand() & 0xf;
    printf("\n%d:", n);
    usleep(100000);
  }
}
