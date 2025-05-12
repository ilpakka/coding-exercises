#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "shell.h"
#include "parser.h"
#include "builtins.h"
#include "lib/history/history.h"
#include "readline_utils.h"

#define MAX_ARGS 10

static void handle_sigint(int sig) {
    (void)sig;
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

int main(void) {
    char *line;
    char *args[MAX_ARGS];
    int status = 1;
    
    // Set up signal handling
    signal(SIGINT, handle_sigint);
    
    // Initialize shell, history, and readline
    init_shell();
    init_history();
    init_readline();
    
    while (status) {
        // Get input using readline
        line = get_input_line();
        if (line == NULL) {
            printf("\n");
            break;
        }
        
        // Skip empty lines
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        
        // Add to readline history
        add_to_readline_history(line);
        
        // Expand history references
        char *expanded = expand_history(line);
        if (expanded == NULL) {
            fprintf(stderr, "No matching history entry\n");
            free(line);
            continue;
        }
        
        // Add command to our history
        add_to_history(line);
        
        // Parse command
        int arg_count = parse_command(expanded, args);
        free(expanded);
        free(line);
        
        if (arg_count == 0) {
            continue;
        }
        
        // Execute command
        status = execute_command(args, arg_count);
    }
    
    cleanup_readline();
    cleanup_history();
    cleanup_shell();
    return 0;
} 