#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 1000
#define HISTORY_FILE ".cshell_history"

// History entry structure
typedef struct {
    char *command;
    int number;
} HistoryEntry;

// Function declarations
void init_history(void);
void cleanup_history(void);
void add_to_history(const char *command);
void save_history(void);
void load_history(void);
void print_history(void);
char *get_history_entry(int number);
int get_history_count(void);

// History expansion functions
char *expand_history(const char *line);
char *get_last_command(void);
char *get_command_by_number(int number);
char *get_command_by_prefix(const char *prefix);

#endif // HISTORY_H 