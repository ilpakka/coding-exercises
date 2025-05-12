#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <stdbool.h>

// Redirection types
typedef enum {
    REDIR_NONE,
    REDIR_IN,      // <
    REDIR_OUT,     // >
    REDIR_APPEND,  // >>
    REDIR_ERR,     // 2>
    REDIR_ERR_APPEND // 2>>
} RedirectionType;

// Structure to hold redirection information
typedef struct {
    RedirectionType type;
    char *filename;
    int fd;  // File descriptor
} Redirection;

// Function declarations
bool parse_redirection(char **args, int *arg_count, Redirection *redir);
void setup_redirection(Redirection *redir);
void cleanup_redirection(Redirection *redir);
void reset_redirection(Redirection *redir);

#endif // REDIRECTION_H 