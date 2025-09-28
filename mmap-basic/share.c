#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int n = 0;

int main () {
  int stat;
  srand(time(NULL));

  if (fork() == 0) {
    // as the child, write a random number to shared memory (TODO!)
    n = rand();
    printf("Child has written %d to address %p\n", n, &n);
    exit(0);
  } else {
    // as the parent, wait for the child and read out its number
    wait(&stat);
    printf("Parent reads %d from address %p\n", n, &n);
  }
}
