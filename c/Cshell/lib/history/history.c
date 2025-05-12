#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "history.h"

static HistoryEntry history[MAX_HISTORY];
static int history_count = 0;
static int history_next = 0;

void init_history(void) {
    // Initialize history array
    for (int i = 0; i < MAX_HISTORY; i++) {
        history[i].command = NULL;
        history[i].number = 0;
    }
    
    // Load history from file
    load_history();
}

void cleanup_history(void) {
    // Free all history entries
    for (int i = 0; i < MAX_HISTORY; i++) {
        free(history[i].command);
    }
    
    // Save history to file
    save_history();
}

void add_to_history(const char *command) {
    // Skip empty commands
    if (command == NULL || strlen(command) == 0) {
        return;
    }
    
    // Free the oldest entry if we're at capacity
    if (history_count == MAX_HISTORY) {
        free(history[history_next].command);
    }
    
    // Add new command
    history[history_next].command = strdup(command);
    history[history_next].number = history_count + 1;
    
    // Update counters
    history_next = (history_next + 1) % MAX_HISTORY;
    if (history_count < MAX_HISTORY) {
        history_count++;
    }
}

void save_history(void) {
    char *home = getenv("HOME");
    if (home == NULL) {
        return;
    }
    
    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/%s", home, HISTORY_FILE);
    
    FILE *file = fopen(history_path, "w");
    if (file == NULL) {
        return;
    }
    
    // Write history entries
    for (int i = 0; i < history_count; i++) {
        int idx = (history_next - history_count + i + MAX_HISTORY) % MAX_HISTORY;
        if (history[idx].command != NULL) {
            fprintf(file, "%s\n", history[idx].command);
        }
    }
    
    fclose(file);
}

void load_history(void) {
    char *home = getenv("HOME");
    if (home == NULL) {
        return;
    }
    
    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/%s", home, HISTORY_FILE);
    
    FILE *file = fopen(history_path, "r");
    if (file == NULL) {
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        add_to_history(line);
    }
    
    fclose(file);
}

void print_history(void) {
    for (int i = 0; i < history_count; i++) {
        int idx = (history_next - history_count + i + MAX_HISTORY) % MAX_HISTORY;
        if (history[idx].command != NULL) {
            printf("%5d  %s\n", history[idx].number, history[idx].command);
        }
    }
}

char *get_history_entry(int number) {
    if (number <= 0 || number > history_count) {
        return NULL;
    }
    
    int idx = (history_next - history_count + (number - 1) + MAX_HISTORY) % MAX_HISTORY;
    return history[idx].command;
}

int get_history_count(void) {
    return history_count;
}

char *get_last_command(void) {
    if (history_count == 0) {
        return NULL;
    }
    int idx = (history_next - 1 + MAX_HISTORY) % MAX_HISTORY;
    return history[idx].command;
}

char *get_command_by_number(int number) {
    if (number <= 0 || number > history_count) {
        return NULL;
    }
    int idx = (history_next - history_count + (number - 1) + MAX_HISTORY) % MAX_HISTORY;
    return history[idx].command;
}

char *get_command_by_prefix(const char *prefix) {
    if (prefix == NULL || strlen(prefix) == 0) {
        return NULL;
    }
    
    // Search from most recent to oldest
    for (int i = history_count - 1; i >= 0; i--) {
        int idx = (history_next - history_count + i + MAX_HISTORY) % MAX_HISTORY;
        if (history[idx].command != NULL && 
            strncmp(history[idx].command, prefix, strlen(prefix)) == 0) {
            return history[idx].command;
        }
    }
    return NULL;
}

char *expand_history(const char *line) {
    if (line == NULL || strlen(line) == 0) {
        return strdup(line);
    }
    
    char *result = strdup(line);
    char *current = result;
    char *expanded = NULL;
    
    while ((current = strstr(current, "!")) != NULL) {
        // Handle !! (last command)
        if (current[1] == '!') {
            char *last_cmd = get_last_command();
            if (last_cmd == NULL) {
                free(result);
                return NULL;
            }
            
            // Replace !! with the last command
            char *new_result = malloc(strlen(result) + strlen(last_cmd) + 1);
            int pos = current - result;
            strncpy(new_result, result, pos);
            strcpy(new_result + pos, last_cmd);
            strcpy(new_result + pos + strlen(last_cmd), current + 2);
            
            free(result);
            result = new_result;
            current = result + pos + strlen(last_cmd);
            continue;
        }
        
        // Handle !n (command by number)
        if (isdigit(current[1])) {
            int num = atoi(current + 1);
            char *cmd = get_command_by_number(num);
            if (cmd == NULL) {
                free(result);
                return NULL;
            }
            
            // Count digits in the number
            int digits = 0;
            char *p = current + 1;
            while (isdigit(*p)) {
                digits++;
                p++;
            }
            
            // Replace !n with the command
            char *new_result = malloc(strlen(result) + strlen(cmd) + 1);
            int pos = current - result;
            strncpy(new_result, result, pos);
            strcpy(new_result + pos, cmd);
            strcpy(new_result + pos + strlen(cmd), current + 1 + digits);
            
            free(result);
            result = new_result;
            current = result + pos + strlen(cmd);
            continue;
        }
        
        // Handle !prefix (command by prefix)
        if (isalnum(current[1])) {
            char *prefix_end = current + 1;
            while (isalnum(*prefix_end)) {
                prefix_end++;
            }
            
            int prefix_len = prefix_end - (current + 1);
            char *prefix = malloc(prefix_len + 1);
            strncpy(prefix, current + 1, prefix_len);
            prefix[prefix_len] = '\0';
            
            char *cmd = get_command_by_prefix(prefix);
            free(prefix);
            
            if (cmd == NULL) {
                free(result);
                return NULL;
            }
            
            // Replace !prefix with the command
            char *new_result = malloc(strlen(result) + strlen(cmd) + 1);
            int pos = current - result;
            strncpy(new_result, result, pos);
            strcpy(new_result + pos, cmd);
            strcpy(new_result + pos + strlen(cmd), prefix_end);
            
            free(result);
            result = new_result;
            current = result + pos + strlen(cmd);
            continue;
        }
        
        // If we get here, it's just a literal '!'
        current++;
    }
    
    return result;
} 