#ifndef BUILTINS_H
#define BUILTINS_H

// Built-in command function declarations
int builtin_cd(char **args, int arg_count);
int builtin_pwd(char **args, int arg_count);
int builtin_exit(char **args, int arg_count);
int builtin_echo(char **args, int arg_count);
int builtin_history(char **args, int arg_count);

#endif // BUILTINS_H 