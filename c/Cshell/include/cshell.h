#ifndef CSHELL_H
#define CSHELL_H

// Version information
#define CSHELL_VERSION "1.0.0"
#define CSHELL_AUTHOR "Your Name"

// Shell configuration
#define CSHELL_MAX_LINE 1024
#define CSHELL_MAX_ARGS 64
#define CSHELL_PROMPT "cshell> "

// Function declarations
int cshell_init(void);
int cshell_run(void);
void cshell_cleanup(void);

#endif // CSHELL_H 