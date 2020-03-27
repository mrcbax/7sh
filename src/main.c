#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

static size_t MAX_LEN = 2000;

int curr_proc;

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

void get_cmd(char * prompt, char **command) {
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
  size_t len = 0;
  *command = malloc(sizeof(char));
  getline(command, &len, stdin);
  strtok(*command, "\n");
}

void process_cmd(char *cmd) {
  int len = strlen(cmd);
  char *prog;
  char **args = NULL;
  char *token = strtok(cmd, " ");
  prog = token;
  int ct = 0;
  while (token != NULL) {
    token = strtok(NULL, " ");
    if (token != NULL) {
      args = realloc(args, (ct + 1)*sizeof(char *));
      args[ct] = token;
      ct++;
    }
  }
  if (strcmp(prog, "exit") == 0) {
    kill_shell();
  } else {
    fork_run_wait(prog, args);
  }
}

int main (int argc, char **argv) {
  signal(SIGINT, sigint_procs); //Register C-c handler.
  signal(SIGTERM, kill_shell);
  char * prompt = "";
  if (argc > 1) {
    prompt = argv[1];
  }
  while(true) {
    char *cmd;
    get_cmd(prompt, &cmd);
    process_cmd(cmd);
  }

  return EXIT_SUCCESS; //The shell shouldn't fail.
}
