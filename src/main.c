#ifdef __linux__
#define _GNU_SOURCE
#endif
#include "common.h"
#include "builtin.h"
//#include <jemalloc/jemalloc.h>
static size_t MAX_LEN = 2000;

int curr_proc;

void sigint_procs() { //easier than registering two signal handlers.
  kill(curr_proc, SIGINT);
}

void kill_shell() { //no longer used. If the user types exit then there is no currently running process so no need to kill those.
  sigint_procs();
  exit(EXIT_SUCCESS);
}

int find_char(char *st, char c) { //locates chars in a string, used for whitespace processing.
  int count = 0;
  for (int i = 0; st[i]; i++) {
    if(st[i] == c) {
      count++;
    }
  }
  return count;
}

// BEGIN Borrowed from teenysh.c
char whitespace[] = " \t\r\n\v";

void cmd_not_found(char *cmd) {
  fprintf(stderr, "7sh: cmd not found: %s\n", cmd);
  exit(EXIT_FAILURE);
}

void fork_run_wait(char *args[]) {
  if ((curr_proc = fork()) == 0) { //store the fork pid so we can C-c it.
    #ifdef __linux__
    char *path = getenv("PATH"); //tired of not having my path.
    char  pathenv[strlen(path) + sizeof("PATH=")]; //ensure the PATH= fits.
    char *envp[] = {pathenv, NULL}; //add a null.
    execvpe(args[0], args, envp);
    #endif
    #ifndef __linux__
    execvp(args[0], args);
    #endif
    cmd_not_found(args[0]);
  }
  wait(&curr_proc);
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
  fflush(stderr);
  fflush(stdin);
  size_t len = 0;
  getline(command, &len, stdin);
  strtok(*command, "\n");
}

void process_cmd(char *cmd) {
  int num_args = 1;
  num_args += find_char(cmd, whitespace[0]);
  num_args += find_char(cmd, whitespace[1]);
  num_args += find_char(cmd, whitespace[2]);
  num_args += find_char(cmd, whitespace[4]);
  char **args = malloc(((num_args + 1)*sizeof(char *)) + ((strlen(cmd))*sizeof(char)));
  char * token = strtok(cmd, " ");
  int ct = 0;
  while (token != NULL) {
    if (token != NULL) {
      args[ct] = token;
      ct++;
      token = strtok(NULL, " ");
    }
  }
  ct++;
  args[ct] = NULL;
  for (int p = 0; p < num_args; p++){
    fprintf(stdout, "%s\t", args[p]);
  }
  fflush(stdout);
  if(is_builtin(args[0])){
    exec_builtin(args);
  } else {
    fork_run_wait(args);
    free(args);
    args = NULL;
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
    free(cmd);
  }

  return EXIT_SUCCESS; //The shell shouldn't fail.
}
