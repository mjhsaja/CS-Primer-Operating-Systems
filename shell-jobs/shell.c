#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROMPT 4096
#define MAX_ARGV 256
#define MAX_CMDS 8
#define PROMPT "\xF0\x9F\x90\x9A "
#define SEP " \t\n"

// TODO update SIGINT forwarding for pipelines
volatile pid_t childpid = 0;

void sigint_handler(int sig) {
  if (!childpid)
    return;
  if (kill(childpid, SIGINT) < 0)
    perror("Error sending SIGINT to child");
  return;
}

int main() {
  char prompt[MAX_PROMPT];
  int argi, cmdi;
  char *cmds[MAX_CMDS][MAX_ARGV];
  char *tok;
  signal(SIGINT, sigint_handler);

  while (1) {
    printf(PROMPT);
    if ((NULL == fgets(prompt, MAX_PROMPT, stdin)) && ferror(stdin)) {
      perror("fgets error");
      exit(1);
    }
    if (feof(stdin))
      exit(0);

    // tokenize
    argi = 0;
    cmdi = 0;

    tok = strtok(prompt, SEP);

    while (1) {
      cmds[cmdi][argi++] = tok;
      if (tok == NULL)
        break;
      tok = strtok(NULL, SEP);
      if (tok && strcmp(tok, "|") == 0) {
        cmds[cmdi++][argi] = NULL;
        argi = 0;
        tok = strtok(NULL, SEP);
      }
    }

    // eval
    if (cmds[0][0] == 0)
      continue;

    if (0 == strcmp(cmds[0][0], "quit"))
      exit(0);
    if (0 == strcmp(cmds[0][0], "help")) {
      printf("Welcome to my shell, builtins are quit and help\n");
      continue;
    }

    // In a loop,
    // Fork and exec every cmd
    // For all but the final cmd, create a pipe
    // e.g. for A | B | C, create pipes alongside A and B
    // Each cmd but the last will write to the newly created pipe's write end
    // Parent will save the read fd for the new pipe, so that the next cmd can
    // read All unused pipe ends should be closed (don't forget duplicated fds
    // on fork)
    // TODO
    // check return vals of dup2 and close
    // clean up after failed fork or exec
    // guard against > MAX_CMDS cmds
    int fds[2];
    int infd = 0;
    int childpids[cmdi + 1];
    for (int i = 0; i <= cmdi; i++) {
      if (i != cmdi)
        pipe(fds);

      if ((childpids[i] = fork()) < 0) {
        perror("fork error");
        exit(1);
      }

      if (childpids[i] == 0) { // child
        if (i != cmdi) {
          dup2(fds[1], 1);
          close(fds[1]);
          close(fds[0]);
        }
        dup2(infd, 0);
        if (execvp(cmds[i][0], cmds[i]) < 0) {
          perror("exec error");
          exit(1);
        }
        exit(1);
      }
      // parent
      infd = fds[0];
      if (i != cmdi)
        close(fds[1]);
    }

    int status;
    for (int i = 0; i <= cmdi; i++) {
      waitpid(childpids[i], &status, 0);
    }
  }
}
