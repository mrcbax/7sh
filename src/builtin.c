#include "builtin.h"

static char *commands[] = {"exit", "egg", "help"};
static int num_commands = 3;

int is_builtin(char *name) {
  for (int i = 0; i < num_commands; i++) { //go trough all the built in commands
    if (strcmp(name, commands[i]) == 0) { //if the command matches a built in command return its number
      return i;
    }
  }
  return -1; //else its not a builtin
}

void exec_builtin(char *args[]) { //executes a built in command
  switch(is_builtin(args[0])) {
  case 0: { //exit command
    exit(EXIT_SUCCESS);
    break;
  }
  case 1: { // easteregg
    fprintf(stdout, "_________  _________ ___ ______________.____    .____     \n\\______  \\/   _____//   |   \\_   _____/|    |   |    |    \n    /    /\\_____  \\/    ~    \\    __)_ |    |   |    |    \n   /    / /        \\    Y    /        \\|    |___|    |___ \n  /____/ /_______  /\\___|_  /_______  /|_______ \\_______ \\\n                 \\/       \\/        \\/         \\/       \\/\n");
    break;
  }
  case 2: { //help command
    fprintf(stdout, "7-Shell: v0.1.0 ©DosLab Electronics, LLC\nLicensed Under GNU GPL3\n\nUsage:\t7sh <prompt_string>\n");
    break;
  }
  }
}
