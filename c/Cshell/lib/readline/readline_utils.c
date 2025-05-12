#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readline_utils.h"
#include "shell.h"

static char *prompt = NULL;

// Custom completion function
char **command_completion(const char *text, int start, int end) {
    char **matches = NULL;
    
    // Only complete at the start of the line
    if (start == 0) {
        matches = rl_completion_matches(text, rl_command_generator);
    }
    
    return matches;
}

// Command generator for completion
char *rl_command_generator(const char *text, int state) {
    static int list_index, len;
    static const char *commands[] = {
        "cd", "pwd", "echo", "exit", "history",
        NULL
    };
    
    if (!state) {
        list_index = 0;
        len = strlen(text);
    }
    
    const char *name;
    while ((name = commands[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    
    return NULL;
}

void init_readline(void) {
    // Set up readline
    rl_readline_name = "cshell";
    rl_attempted_completion_function = command_completion;
    
    // Set up prompt
    prompt = strdup("cshell> ");
    
    // Load history
    char *home = getenv("HOME");
    if (home) {
        char history_path[1024];
        snprintf(history_path, sizeof(history_path), "%s/.cshell_history", home);
        read_history(history_path);
    }
}

char *get_input_line(void) {
    return readline(prompt);
}

void add_to_readline_history(const char *line) {
    if (line && *line) {
        add_history(line);
    }
}

void cleanup_readline(void) {
    // Save history
    char *home = getenv("HOME");
    if (home) {
        char history_path[1024];
        snprintf(history_path, sizeof(history_path), "%s/.cshell_history", home);
        write_history(history_path);
    }
    
    // Clean up
    free(prompt);
} 