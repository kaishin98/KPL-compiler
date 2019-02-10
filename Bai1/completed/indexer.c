#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define WORD_LENGTH 20
#define PAGE_LENGTH 100
#define MAX_LINE_NO 3
#define MAX_LINE_LENGTH 255
#define MAX_WORD 30

typedef struct data {
    char word[WORD_LENGTH];
    int num;    // so lan xuat hien
    char lnum[PAGE_LENGTH][MAX_LINE_NO];  // cac dong xuat hien, toi da co 100 dong
}token;

token stop_token[MAX_WORD]; // toi da co 30 tu cam
token proNoun[MAX_WORD];
int proNoun_range = 0;
int stop_range = 0;

token hash_token['z' - 'a'][MAX_WORD]; // toi da 20 tu co nghia ung voi cac chu cai dau tu a->z
int num['z' - 'a'] = {0};  // so luong tu ung voi cac chu cai dau tu a->z

void init() {
    for(int i = 0; i < 'z' - 'a'; i++) {   
        for (int j = 0; j < MAX_WORD; j++)
        {
            hash_token[i][j].num = 0;
        }
    }
}

void to_lower(char buf[]) {
    int i = 0;
     
    while(buf[i] != '\0') { 
        if(buf[i] >= 'A' && buf[i] <= 'Z') buf[i] += 32;
        i++;
    }    
}

int check_stop_token(char buf[]) {   // ton tai thi tra lai 1, 0 trong th nguoc lai
    int i = 0;

    to_lower(buf);
    while(i < stop_range) {
        if(strcmp(stop_token[i++].word, buf) == 0) return 1;
    }
    return 0;
}

int check_proNoun(char buf[]) {
    int i = 0;

    while(i < proNoun_range) {
        if(strcmp(proNoun[i++].word, buf) == 0) return 1;
    }
    return 0;
}

int get_num_line(int p, int k) {
    for(int i = 0; i < PAGE_LENGTH; i++) 
        if(hash_token[p][k].lnum[i][0] == '\0') return i;
}

void push_hash_list(char buf[], int line_no) {    //  tra ve vi tri tu duoc them vao hoac update trong mang cac ki tu dau 
    int i = 0;
    int k = buf[0] - 97;
    int range = num[k];
    int w, n;

    char s[MAX_LINE_NO];
    sprintf(s, "%d", line_no);  // chuyen so dong sang string
    
    while(i < num[k]) {
        if(strcmp(hash_token[k][i].word, buf) == 0) {    // neu da ton tai trong day
            hash_token[k][i].num += 1;
            break;
        }
        i++;
    }

    if(i == num[k]) {
        strcpy(hash_token[k][i].word, buf);  // neu chua ton tai
        hash_token[k][i].num = 1;
        num[k] += 1;
    }

    n = get_num_line(k, i); // lay ra so luong dong
    for(w = 0; w < n; w++)
        if(strcmp(hash_token[k][i].lnum[w], s) == 0) 
            break;
    if(w == n) strcpy(hash_token[k][i].lnum[w], s);

    return;
}

void push(int op) {
    char line[MAX_LINE_LENGTH];
    char buffer[WORD_LENGTH];
    int index = 0;
    int j = 0;
    int line_no = 1;
    int choice; 

    if(op == 1) {
        FILE *f = fopen("stopw.txt", "r");
        while(fgets(line, MAX_LINE_LENGTH, f)) {
            for(int i = 0; i < strlen(line); i++) {
                if(line[i] >= 'a' && line[i] <= 'z') { 
                    buffer[j++] = line[i];
                }
                else {
                    buffer[j] = '\0'; 
                    if(*buffer != '\0') {
                        strcpy(stop_token[index++].word, buffer);
                        stop_range += 1;
                    }
                    j = 0;
                }            
            }        
        }
        fclose(f);
    }

    else if(op == 2) {
        FILE *f = fopen("vanban.txt", "r");
        while(fgets(line, MAX_LINE_LENGTH, f)) {
            for(int i = 0; i < strlen(line); i++) {
                if((line[i] >= 'a' && line[i] <= 'z')) {    // neu gap ki tu a->z, copy vao buffer
                    buffer[j++] = line[i];
                }
                else if(line[i] >= 'A' && line[i] <= 'Z') { // neu gap ki tu A->Z
                    if(i != 0) {    // neu khong phai ki tu dau tien cua 1 dong
                        if(line[i-2] != '.') {  // la danh tu rieng
                            while(line[i+1] >= 'a' && line[i+1] <= 'z') i++;    //  i tang toi khi het mot tu
                        } else buffer[j++] = line[i];   // khong phai la danh tu rieng 
                    }
                    else {  //  neu la ki tu dau tien cua 1 dong
                        while((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z'))
                            buffer[j++] = line[i++];
                        buffer[j] = '\0';
                        j = 0;
                        char tmp_buffer[WORD_LENGTH];
                        strcpy(tmp_buffer, buffer);

                        if(!check_stop_token(buffer)) {
                            if(!check_proNoun(tmp_buffer)) {
                                printf("%s co phai danh tu rieng khong (1 la dung, 0 la sai):", tmp_buffer);
                                scanf("%d", &choice);
                                switch (choice) {
                                    case 1:
                                        strcpy(proNoun[proNoun_range++].word, tmp_buffer);
                                        break;
                                    case 0:
                                        push_hash_list(buffer, line_no);
                                        break;
                                    default:
                                        break;    
                                }
                            }
                            
                        }  
                    }
                }
                else {
                    buffer[j] = '\0'; 
                    if(*buffer != '\0') {
                        to_lower(buffer);
                        if(!check_stop_token(buffer)) {  // neu khong ton tai trong stop_token
                            push_hash_list(buffer, line_no);
                        }
                    }
                j = 0;
                }           
            }
        line_no += 1;      // tang chi so dong len 1        
        }
        fclose(f);
    }   
}

void swap(token *t1, token *t2) {
    token *tmp = (token *)malloc(sizeof(token));
    *tmp = *t1;
    *t1 = *t2;
    *t2 = *tmp;
    free(tmp);
}

void sort(token *t, int k) {
    int min;

    for(int i = 0; i < num[k] - 1; i++) {
        min = i;
        for(int j = i + 1; j < num[k]; j++) 
            if(strcmp(t[min].word, t[j].word) > 0) 
                min = j;
        swap(t + min, t + i);
    }
}

void print() {
    for(int i = 0; i < 'z' - 'a'; i++) {
        if(num[i] != 0) {
            sort(hash_token[i], i);    
            for(int j = 0; j < num[i]; j++) {
                printf("%s %d", hash_token[i][j].word, hash_token[i][j].num);
                for(int k = 0; k < get_num_line(i, j);)
                    printf(",%s", hash_token[i][j].lnum[k++]);
                printf("\n"); 
            }
        }
    }
}

void main() {
    push(1);    // xay dung stop list
    init();
    push(2);    // xay dung useful list
    print();    // print ket qua
    exit(0);
}
