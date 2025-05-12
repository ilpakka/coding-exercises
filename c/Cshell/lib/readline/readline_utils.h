#ifndef READLINE_UTILS_H
#define READLINE_UTILS_H

#include <readline/readline.h>
#include <readline/history.h>

// Function declarations
void init_readline(void);
char *get_input_line(void);
void add_to_readline_history(const char *line);
void cleanup_readline(void);

#endif // READLINE_UTILS_H 