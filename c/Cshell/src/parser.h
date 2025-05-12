#ifndef PARSER_H
#define PARSER_H

// Function declarations
int parse_command(char *line, char **args);
void free_args(char **args, int count);

#endif // PARSER_H 