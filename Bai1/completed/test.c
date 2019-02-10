#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct pos{
    int row;
    int col;
};

struct token{
    char word[10];   
};

void main() {
    FILE *f = fopen("vanban.txt", "r");
    if(f) {
        write(1, "Khong the mo file\n", 18);
        return;
    }
    fclose(f);
    return;
}

