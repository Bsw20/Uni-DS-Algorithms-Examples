#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd;

    char file_name[1024] = "new_foulder/file.txt";
    if ((fd = open(file_name, O_CREAT | O_EXCL, 0666)) < 0) {
        printf("Can\'t create file\n");
        exit(-1);
    }
    
    char first[1024] = "file.txt";
    char second[1024] = "file";
    char str[1024] = "new_foulder/";
    int num = 3;
    do {
        strcpy(str, "new_foulder/");
        strcat(str, second);
        if (symlink(first, str) < 0) {
            printf("Error while creating a directory\n");
            exit(-1);
        }
        ++num;
        size_t new_str_size = sizeof(second);

        strncpy(first, second, new_str_size);
        first[new_str_size - 1] = '\0';
        
        second[num + 1] = '\0';
        second[num] = '3';

        if (close(fd) < 0) {
            exit(-1);
        }
    } while ((fd = open(str, O_RDONLY, 0666)) >= 0);

    printf("Final result: %d\n", num - 1);
    return 0;
}

