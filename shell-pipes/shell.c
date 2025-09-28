#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CMD 4096
#define MAX_ARGV 256
#define PROMPT "\xF0\x9F\x90\x9A "
#define SEP " \t\n"

volatile pid_t childpid = 0;

void sigint_handler(int sig) {
  if (!childpid)
    return;
  if (kill(childpid, SIGINT) < 0)
    perror("Error sending SIGINT to child");
  return;
}

int main() {
  char cmd[MAX_CMD];
  int argc;
  char *argv[MAX_ARGV];

  signal(SIGINT, sigint_handler);

  while (1) {
    printf(PROMPT);
    if ((NULL == fgets(cmd, MAX_CMD, stdin)) && ferror(stdin)) {
      perror("fgets error");
      exit(1);
    }
    if (feof(stdin))
      exit(0);

    // tokenize
    argc = 0;
    argv[argc] = strtok(cmd, SEP);

    while (argv[argc] != NULL) {
      argv[++argc] = strtok(NULL, SEP);
    }

    // eval
    if (argc == 0)
      continue;

    if (0 == strcmp(argv[0], "quit"))
      exit(0);
    if (0 == strcmp(argv[0], "help")) {
      printf("Welcome to my shell, builtins are quit and help\n");
    }

    if ((childpid = fork()) < 0) {
      perror("fork error");
      continue;
    }

    if (childpid == 0) { // child
      if (execvp(argv[0], argv) < 0) {
        perror("exec error");
        exit(1);
      }
      exit(1);
    }

    // parent
    int status;
    waitpid(childpid, &status, 0);
    childpid = 0;
  }
}
