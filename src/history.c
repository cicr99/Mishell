#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../include/parser.h"

#define streq(x, y) (strcmp(x, y) == 0)

#define true 1
#define false 0
#define H_TOP 50


/*
Given a path and a file_name it returns the concatenation of the two of them
*/
char *BuildPath(char *path, char *file_name){
    int path_sz = strlen(path);
    int file_sz = strlen(file_name);
    char *new_path = malloc((path_sz + file_sz + 5) * sizeof(char));
    int pos = 0;
    for(int i = 0; i < path_sz; ++i){
        new_path[pos++] = path[i];
    }
    for (int i = 0; i < file_sz; ++i){
        new_path[pos++] = file_name[i];
    }
    new_path[pos] = '\0';
    return new_path;
}

/*
If the file history doesn't exist, then it creates one.
It copies the content of the file history.txt into history.
*/
char** init_history(char* path, int* length){
    int fd = open(path, O_CREAT|O_RDONLY, 0664);
    char** history = malloc(H_TOP * sizeof(char*));
    int history_sz = 0;

    int line_sz = 50, pos = 0, complete = true;
    char* line = malloc(line_sz * sizeof(char));
    char c, symbol;

    while(read(fd, &c, 1)){
        if(pos == line_sz){
            line_sz *= 2;
            line = realloc(line, line_sz * sizeof(char));
        }
        line[pos++] = c;

        //there is a word between quotes, so it needs to be read until finding the closing quote
        if(c == '"' || c == '\''){
            if(complete){
                complete = false;
                symbol = c;
            }
            else if(c == symbol)
                complete = true;
        }

        //it got to the end of the line
        if(c == '\n' && complete){
            line[pos - 1] = '\0';
            history[history_sz++] = line;
            line_sz = 50, pos = 0;
            line = malloc(line_sz * sizeof(char));
        }
    }

    close(fd);
    *length = history_sz;
    return history;
}

/*
The param input does not start with a space
It checks that input is different from the last command line saved in history
It adds the new input if the condition was true
If there are already H_TOP lines saved in history, it pops the first one
*/
void AddToHist(char* input, char** history, int* length, char* path){
    int history_sz = *length;
    if(history_sz == 0 || !streq(input, history[history_sz - 1])){
        int pos = history_sz;
        if(history_sz == H_TOP){
            for(int i = 1; i < history_sz; ++i)
                history[i - 1] = history[i];
            pos = history_sz - 1;
            history_sz--;
        }
        history[pos] = input;
        ++history_sz;

        int fd = open(path, O_CREAT|O_TRUNC|O_WRONLY, 0664);
        for(int i = 0; i < history_sz; ++i){
            write(fd, history[i], strlen(history[i]));
            write(fd, "\n", 1);
        }
        close(fd);
    }
    *length = history_sz;
}


int SpecialChar(char c){
    return (c == ' ') || (c == '>') || (c == '<') || (c == '|') || (c == ';');
}


char* GetLastOne(char** history, int history_sz, int* len){
    if(history_sz == 0){
        *len = 0;
        return NULL;
    }
    char* aux = history[history_sz - 1];
    *len = strlen(aux);
    return aux;
}

char* GetNumber(int number,char** history, int history_sz, int* len){
    if(number < 1 || number > history_sz){
        *len = 0;
        return NULL;
    }
    char* aux = history[number - 1];
    int sz = strlen(aux);
    *len = sz;
    if(aux[sz - 1] == '\n')
        aux[sz - 1] = '\0';
    return aux;
}

char* GetLastCommand(char* input, int start, int fin, char** history, int history_sz, int* len){
    int found = false;
    int i = history_sz - 1;
    for (; i >= 0; i--){
        if(strncmp(input + start, history[i], fin - start) == 0){
            found = true;
            break;
        }
    }
    if(!found){
        *len = 0;
        return NULL;
    }
    *len = strlen(history[i]);
    return history[i];
}

char* ReplaceInput(char *input, char** history, int history_sz, int* new_sz){
    int i = 0, pos = 0, complete = true, aux_sz = 0, number, changed = false;
    int sz = strlen(input);
    char* new_in = malloc(sz * 5 * sizeof(char));
    char* aux;
    char** endp;
    char c, symbol;

    while(i < sz){
        c = input[i];
        if (c == '"' || c == '\''){
            if (complete){
                complete = false;
                symbol = c;
            }
            else if (c == symbol)
                complete = true;
        }

        if(c == '!' && complete){
            changed = true;
            if(i + 1 < sz && input[i + 1] == '!'){
                aux = GetLastOne(history, history_sz, &aux_sz);
                i += 2;
            }
            else{
                endp = malloc(5 * sizeof(char *));
                number = strtol(input + i + 1, endp, 10);
                i = *endp - input;

                if(number != 0)
                    aux = GetNumber(number, history, history_sz, &aux_sz);
                else{
                    int j = i;
                    while(j < sz && !(SpecialChar(input[j]) && complete))
                        ++j;
                    aux = GetLastCommand(input, i, j, history, history_sz, &aux_sz);
                    i = j;
                }

            }
            for(int k = 0; k < aux_sz; ++k)
                new_in[pos++] = aux[k];
        }
        else{
            new_in[pos++] = c;
            ++i;
        }
    }

    //if there was any command ! then it prints the replaced line
    if(changed)
        printf("%s\n", new_in);

    *new_sz = pos;
    return new_in;
}