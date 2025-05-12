#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/cshell.h"

void test_shell_init(void) {
    int result = cshell_init();
    assert(result == 0);
    printf("Shell initialization test passed\n");
}

void test_shell_cleanup(void) {
    cshell_cleanup();
    printf("Shell cleanup test passed\n");
}

int main(void) {
    printf("Running CShell tests...\n");
    
    test_shell_init();
    test_shell_cleanup();
    
    printf("All tests passed!\n");
    return 0;
} 