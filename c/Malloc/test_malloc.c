#include "mr_malloc.h"
#include <stdio.h>
#include <string.h>

void test_basic_allocation() {
    printf("\nTesting basic allocation...\n");
    
    // Basic allocation
    int* arr = (int*)mr_malloc(5 * sizeof(int));
    if (arr) {
        printf("Basic allocation successful\n");
        for (int i = 0; i < 5; i++) {
            arr[i] = i;
        }
        mr_free(arr);
        printf("Basic free successful\n");
    }

    // Multiple allocations
    char* str1 = (char*)mr_malloc(10);
    char* str2 = (char*)mr_malloc(20);
    
    if (str1 && str2) {
        strcpy(str1, "Hello");
        strcpy(str2, "World!");
        printf("Multiple allocations successful\n");
        printf("str1: %s, str2: %s\n", str1, str2);
        
        mr_free(str1);
        mr_free(str2);
        printf("Multiple frees successful\n");
    }

    print_memory_map();
}

void test_edge_cases() {
    printf("\nTesting edge cases...\n");
    
    // Zero size allocation
    void* ptr = mr_malloc(0);
    if (ptr == NULL) {
        printf("Zero size allocation correctly returned NULL\n");
    }

    // Free NULL
    mr_free(NULL);
    printf("Freeing NULL pointer handled correctly\n");

    // Large allocation
    void* large = mr_malloc(1024 * 1024); // 1MB
    if (large) {
        printf("Large allocation successful\n");
        mr_free(large);
    }

    print_memory_map();
}

int main() {
    printf("Starting malloc tests...\n");
    
    test_basic_allocation();
    test_edge_cases();
    
    printf("\nAll tests completed!\n");
    return 0;
} 