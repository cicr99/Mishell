#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>

#include "../include/cd.h"

#define _GNU_SOURCE
#define len(x) strlen(x)
#define streq(x, y) (strcmp(x, y) == 0)

#define true 1
#define false 0
#define BUFFER_SIZE 4096


char* special_symbol = "$";
int *_process1;
int *_process2;

/*returns a set of strings which is the result of separating input by each character of delimiter*/
char** Parser(char* input, char* delimiter, int* total){
    int cant = 0;
    char** programs = malloc(len(input) * sizeof(char*));
    char* program = strtok(input, delimiter);

    while(program){
        programs[cant++] = program;
        program = strtok(NULL, delimiter);
    }

    *total = cant;

    return programs;

}

/*
Creates a new array cloned which copies input
Cloned contains spaces in every position of input that is not between quotes
  and contains the string that is between them
It replaces in input the string that is between quotes with a special symbol
It also eliminates the quotes
*/
char* RemoveStrings(char** old_input){
    char* input = *old_input;
    int sz = len(input), pos = 0;
    char* new_in = malloc(sz * sizeof(char));
    char* cloned = malloc(sz * sizeof(char));

    for(int i = 0; i < sz; ++i){
        char c = input[i];
        if(c == '"' || c == '\''){
            while(input[++i] != c){
                cloned[pos] = input[i];
                new_in[pos++] = special_symbol[0];
            }
        }
        else if(c == '\n'){
            new_in[pos] = ' ';
            cloned[pos++] = ' ';
        }
        else if(c == 92){
            c = input[++i];
            cloned[pos] = c;
            new_in[pos++] = special_symbol[0];
        }
        else{
            cloned[pos] = ' ';
            new_in[pos++] = c;
        }
    }
    *old_input = new_in;
    return cloned;
}

char** RedirParser(char** words, int* cant){
    char** params = malloc(BUFFER_SIZE * sizeof(char*));
    int pos = 0, start, cant_w, cant_s, pos_w, pos_s, sz_w = *cant;

    for(int i = 0; i < sz_w; ++i){
        int sz = len(words[i]);
        char* cloned = malloc(sz * sizeof(char));
        start = (words[i][0] == '<' || words[i][0] == '>');
        for(int j = 0; j < sz; ++j){
            char c = words[i][j];
            cloned[j] = (c == '<' || c == '>') ? c : special_symbol[0];
        }

        char** w = Parser(words[i], "<>", &cant_w);
        char** s = Parser(cloned, special_symbol, &cant_s);
        pos_s = pos_w = 0;
        for(int j = 0; j < cant_s + cant_w; ++j){
            params[pos++] = (start) ? s[pos_s++] : w[pos_w++];
            start = !start;
        }
    }
    *cant = pos;
    return params;
}

void RemoveA(char** words, int* cant){
    int sz = *cant;
    if(streq(words[sz - 1], "&")){
        words[sz - 1][0] = '\0';
        *cant = sz - 1;
    }
    else if(words[sz - 1][strlen(words[sz - 1]) - 1] == '&')
        words[sz - 1][strlen(words[sz - 1]) - 1] = '\0';
}

char** ParseCommand(char* command, char* input, char* cloned, int* total){
    int cantWords = 0;
    char **words = Parser(command, " ", &cantWords);

    RemoveA(words, &cantWords);
    words = RedirParser(words, &cantWords);

    for (int i = 0; i < cantWords; ++i){
        int sz = len(words[i]);
        for (int j = 0; j < sz; ++j){
            if (words[i][j] == special_symbol[0])
                words[i][j] = cloned[words[i] - input + j];
        }
    }

    *total = cantWords;
    return words;
}

void ExecHistory(char **history, int history_sz){
    for (int i = 1; i <= history_sz; ++i)
        printf("%d %s\n", i, history[i - 1]);
    exit(0);
}

void RunCommand(char** words, int cantWords, char** history, int history_sz){
    char** args = malloc(cantWords * sizeof(char*));
    int pos = 0, fdOut, fdIn;

    fdIn = dup(STDIN_FILENO);
    fdOut = dup(STDOUT_FILENO);
    for(int i = 0; i < cantWords; ++i){
        if (streq(words[i], ">"))
            fdOut = open(words[++i], O_CREAT | O_TRUNC | O_WRONLY, 0664);
        else if (streq(words[i], ">>"))
            fdOut = open(words[++i], O_CREAT | O_APPEND | O_WRONLY, 0664);
        else if (streq(words[i], "<"))
            fdIn = open(words[++i], O_RDONLY, 0664);
        else
            args[pos++] = words[i];
    }
    dup2(fdIn, STDIN_FILENO);
    dup2(fdOut, STDOUT_FILENO);

    if(streq(args[0], "ls") || streq(args[0], "grep"))
        args[pos++] = "--color=auto";

    args[pos] = NULL;

    if(streq(args[0], "history")){
        ExecHistory(history, history_sz);
    }
    else{
        //TODO: coger errores
        int status = execvp(args[0], args);
        perror(args[0]);
    }

}



void RunProgram(char* program, char* input, char* cloned, char** history, int history_sz){
    int cantCommand = 0, pipefd[2], pid;
    char** commands = Parser(program, "|", &cantCommand);
    char** args;
    int argc;

    for (int i = 0; i < cantCommand; ++i){
        args = malloc(len(commands[i]) * sizeof(char*));
        argc = 0;
        args = ParseCommand(commands[i], input, cloned, &argc);

        pipe(pipefd);
        pid = fork();
        if(pid){
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            waitpid(pid, NULL, 0); 
        }
        else{
            close(pipefd[0]);
            if(i != cantCommand - 1)
                dup2(pipefd[1], STDOUT_FILENO);
            RunCommand(args, argc, history, history_sz);
            exit(0);
        }
    }
}

/*Transforms the input line so all strings between quotes are now replaced by a special symbol
Separates the new input line by ;
Executes RunProgram for every segment of the input*/
void RunLine(char* input, char** history, int history_sz, int *process1, int *process2){
    _process1 = process1;
    _process2 = process2;

    char* cloned = RemoveStrings(&input);
    int cantProgram = 0, fdIn;
    char** programs = Parser(input, ";", &cantProgram);

    char *programs2 = malloc(len(programs[0]) * sizeof(char));
    for(int i = 0; i < len(programs[0]); i++)
        programs2[i] = programs[0][i];

    char **args2;
    int temp;
    args2 = Parser(programs2, " ", &temp);


    if (streq(args2[0], "cd")){
        cd(args2, temp);
        return;
    }

    if (streq(args2[0], "fg")){
        if(temp == 1){
            printf("Arguments are missing\n");
            return;
        }

        printf("ID: %d \n", atoi(args2[1]));
        waitpid(atoi(args2[1]), NULL, 0);
        return;
    }

    if (streq(args2[0], "jobs")){
        int status2;
        for(int i = 0; i < 100; i ++){
            if(_process1[i] == -2 && _process2[i] == -2)
                break;
            if(_process2[i] > 0)
                _process1[i] = waitpid(_process2[i], &status2, WNOHANG);

            if(_process1[i] > 0){
                for(int j = i + 1; j < 100; j++){
                    if(_process1[j] == -2 && _process2[j] == -2){
                        _process1[i] = -2;
                        _process2[i] = -2;
                        break;
                    }

                    _process1[j] = waitpid(_process2[i], &status2, WNOHANG);
                    if(_process1[j] == -1){
                        int temp = _process1[i];

                        _process2[i] = _process2[j];
                        _process1[i] = -1;

                        _process1[j] = temp;
                        _process2[j] = temp;
                    }
                }
            }
        }

        for(int i = 0; i < 100; i++){
            if(_process1[i] == -2 && _process2[i] == -2)
                break;
            printf("[%d] ID: %d\n", i + 1, _process2[i]);
        }
        return;
    }

    fdIn = dup(STDIN_FILENO);
    for(int i = 0; i < cantProgram; ++i){
        int status;
        int j = 0;
        for(; j < 100; j++){
            if(_process1[j] == -2 && _process2[j] == -2)
                break;

            _process1[j] = waitpid(_process2[j], &status, WNOHANG);

            if(_process1[j] > 0 || _process1[j] == -1){
                _process1[j] = 0;
                _process2[j] = -2;
                break;
            }
        }

        int pid = fork();

        if(pid){
            char *programs2 = malloc(len(programs[i]) * sizeof(char));
            for(int k = 0; k < len(programs[i]); k++)
                programs2[k] = programs[i][k];

            char **args2;
            int temp;
            args2 = Parser(programs2, " ", &temp);

            if('&' != args2[temp - 1][len(args2[temp - 1]) - 1]) //el ultimmo caracter de la ultima
                waitpid(pid, NULL, 0);
            else{
                _process2[j] = pid;
                printf("[%d]", j + 1);
                printf("ID: %d\n", pid);
            }
        }
        else{
            dup2(fdIn, STDIN_FILENO);
            RunProgram(programs[i], input, cloned, history, history_sz);
            exit(0);
        }
    }
}

/*
Returns false if there is an open quote that hasn't been closed
or if there is a pipe at the end of the line.
Otherwise returns true
*/
int isComplete(char* line, int sz, int last_pipe){
    int complete = true;
    char c;

    int i = 0;
    for(; i < sz; ++i){
        c = line[i];
        if(c == '"' || c == '\''){
            complete = false;
            ++i;
            while(i < sz){
                if(line[i] == c){
                    complete = true;
                    break;
                }
                ++i;
            }
        }
    }
    if(complete && last_pipe)
        complete = false;
    return complete;
}
