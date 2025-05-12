#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "redirection.h"

bool parse_redirection(char **args, int *arg_count, Redirection *redir) {
    redir->type = REDIR_NONE;
    redir->filename = NULL;
    redir->fd = -1;
    
    // Find redirection operator
    for (int i = 0; i < *arg_count; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (i + 1 >= *arg_count) {
                fprintf(stderr, "syntax error: missing filename after '<'\n");
                return false;
            }
            redir->type = REDIR_IN;
            redir->filename = strdup(args[i + 1]);
            // Remove redirection operator and filename from args
            memmove(&args[i], &args[i + 2], (*arg_count - i - 2) * sizeof(char *));
            *arg_count -= 2;
            return true;
        }
        else if (strcmp(args[i], ">") == 0) {
            if (i + 1 >= *arg_count) {
                fprintf(stderr, "syntax error: missing filename after '>'\n");
                return false;
            }
            redir->type = REDIR_OUT;
            redir->filename = strdup(args[i + 1]);
            memmove(&args[i], &args[i + 2], (*arg_count - i - 2) * sizeof(char *));
            *arg_count -= 2;
            return true;
        }
        else if (strcmp(args[i], ">>") == 0) {
            if (i + 1 >= *arg_count) {
                fprintf(stderr, "syntax error: missing filename after '>>'\n");
                return false;
            }
            redir->type = REDIR_APPEND;
            redir->filename = strdup(args[i + 1]);
            memmove(&args[i], &args[i + 2], (*arg_count - i - 2) * sizeof(char *));
            *arg_count -= 2;
            return true;
        }
        else if (strcmp(args[i], "2>") == 0) {
            if (i + 1 >= *arg_count) {
                fprintf(stderr, "syntax error: missing filename after '2>'\n");
                return false;
            }
            redir->type = REDIR_ERR;
            redir->filename = strdup(args[i + 1]);
            memmove(&args[i], &args[i + 2], (*arg_count - i - 2) * sizeof(char *));
            *arg_count -= 2;
            return true;
        }
        else if (strcmp(args[i], "2>>") == 0) {
            if (i + 1 >= *arg_count) {
                fprintf(stderr, "syntax error: missing filename after '2>>'\n");
                return false;
            }
            redir->type = REDIR_ERR_APPEND;
            redir->filename = strdup(args[i + 1]);
            memmove(&args[i], &args[i + 2], (*arg_count - i - 2) * sizeof(char *));
            *arg_count -= 2;
            return true;
        }
    }
    
    return true;
}

void setup_redirection(Redirection *redir) {
    if (redir->type == REDIR_NONE) {
        return;
    }
    
    int flags;
    mode_t mode = 0644;
    
    switch (redir->type) {
        case REDIR_IN:
            redir->fd = open(redir->filename, O_RDONLY);
            if (redir->fd < 0) {
                perror(redir->filename);
                return;
            }
            if (dup2(redir->fd, STDIN_FILENO) < 0) {
                perror("dup2");
                return;
            }
            break;
            
        case REDIR_OUT:
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            redir->fd = open(redir->filename, flags, mode);
            if (redir->fd < 0) {
                perror(redir->filename);
                return;
            }
            if (dup2(redir->fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                return;
            }
            break;
            
        case REDIR_APPEND:
            flags = O_WRONLY | O_CREAT | O_APPEND;
            redir->fd = open(redir->filename, flags, mode);
            if (redir->fd < 0) {
                perror(redir->filename);
                return;
            }
            if (dup2(redir->fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                return;
            }
            break;
            
        case REDIR_ERR:
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            redir->fd = open(redir->filename, flags, mode);
            if (redir->fd < 0) {
                perror(redir->filename);
                return;
            }
            if (dup2(redir->fd, STDERR_FILENO) < 0) {
                perror("dup2");
                return;
            }
            break;
            
        case REDIR_ERR_APPEND:
            flags = O_WRONLY | O_CREAT | O_APPEND;
            redir->fd = open(redir->filename, flags, mode);
            if (redir->fd < 0) {
                perror(redir->filename);
                return;
            }
            if (dup2(redir->fd, STDERR_FILENO) < 0) {
                perror("dup2");
                return;
            }
            break;
            
        default:
            break;
    }
}

void cleanup_redirection(Redirection *redir) {
    if (redir->fd >= 0) {
        close(redir->fd);
        redir->fd = -1;
    }
    if (redir->filename) {
        free(redir->filename);
        redir->filename = NULL;
    }
    redir->type = REDIR_NONE;
}

void reset_redirection(Redirection *redir) {
    if (redir->type == REDIR_NONE) {
        return;
    }
    
    // Restore original file descriptors
    switch (redir->type) {
        case REDIR_IN:
            dup2(STDIN_FILENO, redir->fd);
            break;
        case REDIR_OUT:
        case REDIR_APPEND:
            dup2(STDOUT_FILENO, redir->fd);
            break;
        case REDIR_ERR:
        case REDIR_ERR_APPEND:
            dup2(STDERR_FILENO, redir->fd);
            break;
        default:
            break;
    }
    
    cleanup_redirection(redir);
} 