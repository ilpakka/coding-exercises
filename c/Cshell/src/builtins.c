#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"
#include "shell.h"
#include "history.h"

int builtin_cd(char **args, int arg_count) {
    if (arg_count > 2) {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }
    
    const char *dir;
    if (arg_count == 1 || strcmp(args[1], "~") == 0) {
        dir = getenv("HOME");
    } else {
        dir = args[1];
    }
    
    if (chdir(dir) != 0) {
        perror("cd");
        return 1;
    }
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        set_current_dir(cwd);
    }
    
    return 0;
}

int builtin_pwd(char **args, int arg_count) {
    if (arg_count > 1) {
        fprintf(stderr, "pwd: too many arguments\n");
        return 1;
    }
    
    printf("%s\n", get_current_dir());
    return 0;
}

int builtin_exit(char **args, int arg_count) {
    if (arg_count > 2) {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
    
    int status = 0;
    if (arg_count == 2) {
        status = atoi(args[1]);
    }
    
    exit(status);
    return 0; // never ever reached
}

int builtin_echo(char **args, int arg_count) {
    for (int i = 1; i < arg_count; i++) {
        printf("%s", args[i]);
        if (i < arg_count - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

int builtin_history(char **args, int arg_count) {
    if (arg_count > 2) {
        fprintf(stderr, "history: too many arguments\n");
        return 1;
    }
    
    if (arg_count == 1) {
        // Show all history
        print_history();
    } else {
        // Show last N entries
        int n = atoi(args[1]);
        if (n <= 0) {
            fprintf(stderr, "history: invalid number of entries\n");
            return 1;
        }
        
        int start = history_count - n;
        if (start < 0) start = 0;
        
        for (int i = start; i < history_count; i++) {
            int idx = (history_next - history_count + i + MAX_HISTORY) % MAX_HISTORY;
            if (history[idx].command != NULL) {
                printf("%5d  %s\n", history[idx].number, history[idx].command);
            }
        }
    }
    
    return 0;
} 