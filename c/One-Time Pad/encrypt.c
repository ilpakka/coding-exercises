#include <stdio.h>
#include <stdlib.h>

// One-time pad encryption shenanigans

void encrypt(FILE* file, FILE* encrypted_file, FILE* key_file) {

    int c;
    while ((c=fgetc(file)) != EOF) {
        int key = rand(); // random key
        int encrypted_c = c ^ key; // XOR

        fputc(key, key_file);
        fputc(encrypted_c, encrypted_file);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Gib file for encryption");
    }
    else {
        char* file_name = argv[1];
        FILE* file = fopen(file_name, "r"); // read only
        FILE* encrypted_file = fopen("encrypted.out", "w"); // write only
        FILE* key = fopen("key.out", "w"); // write only

        encrypt(file, encrypted_file, key);

        // close files
        fclose(file);
        fclose(encrypted_file);
        fclose(key);
    }
}