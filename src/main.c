#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <jemalloc/jemalloc.h>

static size_t MAX_LEN = 2000;

int curr_proc;
char *curr_cmd[2000]; //MAX_LEN. linter was complaining.

void sigint_procs() {
  kill(curr_proc, SIGINT);
}

void kill_shell() {
  sigint_procs();
  exit(EXIT_SUCCESS);
}

// BEGIN Borrowed from teenysh.c
char whitespace[] = " \t\r\n\v";

void cmd_not_found(char *cmd) {
  fprintf(stderr, "7sh: cmd not found: %s\n", cmd);
  exit(1);
}

void fork_run_wait(char *cmd, char **args) {
  if ((curr_proc = fork()) == 0) { //store the fork pid so we can C-c it.
    execvp(cmd, args);
    cmd_not_found(cmd);
  }
  wait(NULL);
}
// END Borrowed from teenysh.c

void get_cmd(char * prompt) {
  if (strlen(prompt) > 0) {
    fprintf(stdout, "%s", prompt);// Print custom prompt.
  } else {
    char username[MAX_LEN];
    char hostname[MAX_LEN];
    char cwd[MAX_LEN];
    getlogin_r(username, MAX_LEN - 2); // BEGIN From unistd docs.
    gethostname(hostname, MAX_LEN - 2);
    getcwd(cwd, MAX_LEN - 2); // END From unistd docs.
    fprintf(stdout, "[%s@%s] (%s) ⇶ ", username, hostname, cwd );// Print the prompt
  }
  fflush(stdout);
  fflush(stdin);
  getline(curr_cmd, &MAX_LEN, stdin); //Read command into the curr_cmd string.
}

void process_cmd() {

}

int main (int argc, char **argv) {
  signal(SIGINT, sigint_procs); //Register C-c handler.
  signal(SIGTERM, kill_shell);
  char * prompt = "";
  if (argc > 1) {
    prompt = argv[1];
  }
  while(true) {
    get_cmd(prompt);
    process_cmd();
  }

  return EXIT_SUCCESS; //The shell shouldn't fail.
}
