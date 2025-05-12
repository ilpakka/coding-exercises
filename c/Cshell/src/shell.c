#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include "shell.h"
#include "builtins.h"
#include "lib/history/history.h"
#include "redirection/redirection.h"

static ShellState shell_state;

void init_shell(void) {
    // Get home directory
    struct passwd *pw = getpwuid(getuid());
    shell_state.home_dir = strdup(pw->pw_dir);
    
    // Initialize current directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        shell_state.current_dir = strdup(cwd);
    } else {
        shell_state.current_dir = strdup(shell_state.home_dir);
    }
    
    // Set default prompt
    shell_state.prompt = strdup("cshell> ");
    shell_state.last_status = 0;
    shell_state.running = true;
}

void cleanup_shell(void) {
    free(shell_state.prompt);
    free(shell_state.home_dir);
    free(shell_state.current_dir);
}

void print_prompt(void) {
    printf("%s", shell_state.prompt);
    fflush(stdout);
}

int execute_command(char **args, int arg_count) {
    if (arg_count == 0) {
        return 1;
    }
    
    // Parse redirection
    Redirection redir;
    if (!parse_redirection(args, &arg_count, &redir)) {
        return 1;
    }
    
    // Check for built-in commands
    if (strcmp(args[0], "cd") == 0) {
        int status = builtin_cd(args, arg_count);
        cleanup_redirection(&redir);
        return status;
    } else if (strcmp(args[0], "pwd") == 0) {
        int status = builtin_pwd(args, arg_count);
        cleanup_redirection(&redir);
        return status;
    } else if (strcmp(args[0], "exit") == 0) {
        int status = builtin_exit(args, arg_count);
        cleanup_redirection(&redir);
        return status;
    } else if (strcmp(args[0], "echo") == 0) {
        int status = builtin_echo(args, arg_count);
        cleanup_redirection(&redir);
        return status;
    } else if (strcmp(args[0], "history") == 0) {
        int status = builtin_history(args, arg_count);
        cleanup_redirection(&redir);
        return status;
    }
    
    // External command
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        cleanup_redirection(&redir);
        return 1;
    } else if (pid == 0) {
        // Child
        setup_redirection(&redir);
        if (execvp(args[0], args) < 0) {
            perror("Command not found");
            cleanup_redirection(&redir);
            exit(1);
        }
    } else {
        // Parent
        int status;
        waitpid(pid, &status, 0);
        shell_state.last_status = WEXITSTATUS(status);
    }
    
    cleanup_redirection(&redir);
    return 1;
}

char *get_current_dir(void) {
    return shell_state.current_dir;
}

void set_current_dir(const char *dir) {
    free(shell_state.current_dir);
    shell_state.current_dir = strdup(dir);
}

int get_last_status(void) {
    return shell_state.last_status;
}

void set_last_status(int status) {
    shell_state.last_status = status;
} 