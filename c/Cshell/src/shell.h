#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

// Shell state
typedef struct {
    char *prompt;
    char *home_dir;
    char *current_dir;
    int last_status;
    bool running;
} ShellState;

// Function declarations
void init_shell(void);
void cleanup_shell(void);
void print_prompt(void);
int execute_command(char **args, int arg_count);
char *get_current_dir(void);
void set_current_dir(const char *dir);
int get_last_status(void);
void set_last_status(int status);

#endif // SHELL_H 