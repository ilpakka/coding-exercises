#include <stdio.h>
#include <stdlib.h>

// DECRYPTION

void decryption(FILE* file, FILE* key, FILE* decrypted_file) {

    int encrypted_char;
    
    while ((encrypted_char = fgetc(file)) != EOF) {
        int key_char = fgetc(key);
        int decrypted_char = encrypted_char ^ key_char; // XOR
        fputc(decrypted_char, decrypted_file);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Provide 3 input arguments\n");
    }
    else {
        FILE* file = fopen(argv[1], "r"); // read only;
        FILE* key = fopen(argv[2], "r"); // read only;
        FILE* decrypted_file = fopen("decrypted.out", "w"); // write only

        decryption(file, key, decrypted_file);

        fclose(file);
        fclose(key);
        fclose(decrypted_file);
    }

}