#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define MAX_ARGS 10
#define MAX_ARG_LENGTH 256

int parse_command(char *line, char **args) {
    int count = 0;
    char *token;
    char *saveptr;
    
    // Skip leading whitespace
    while (isspace(*line)) line++;
    
    // If line is empty after skipping whitespace, return 0
    if (*line == '\0') {
        return 0;
    }
    
    // Tokenize the line
    token = strtok_r(line, " \t", &saveptr);
    while (token != NULL && count < MAX_ARGS) {
        args[count] = strdup(token);
        count++;
        token = strtok_r(NULL, " \t", &saveptr);
    }
    
    // Null terminate the argument list
    args[count] = NULL;
    
    return count;
}

void free_args(char **args, int count) {
    for (int i = 0; i < count; i++) {
        free(args[i]);
    }
} 