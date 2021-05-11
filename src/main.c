#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include "../include/cd.h"
#include "../include/parser.h"
#include "../include/history.h"

#define BUFFERSIZE 8192

#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"
#define BLUE "\x1b[34m" BOLD
#define GREEN "\x1b[32m" BOLD


#define true 1
#define false 0

char *cwd;
char *user;
char *pc_name;
int *process1;
int process2[100];

int main(int argc, char **argv)
{
    process1 = malloc(100 * sizeof(int));

    for(int i = 0; i < 100; i++)
    {
        process1[i] = -2;
        process2[i] = -2;
    }

    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);

    user = "mishell";
    pc_name = getenv("USER");

    DefaultPath("/home/", pc_name);

    int history_sz = 0;
    char* history_path = BuildPath(getcwd(buffer, BUFFERSIZE), "/history.txt");
    char** history = init_history(history_path, &history_sz);

    int fdin = dup(STDIN_FILENO);

    while(1)
    {
        char *line = malloc(1000 * sizeof(char));
        cwd = getcwd(buffer, BUFFERSIZE);

        printf(GREEN"%s@%s"RESET":"BLUE"%s"RESET"$", user, pc_name, cwd);
        //printf("%s@%s:%s$", user, pc_name, cwd);
        fflush(NULL);

        int pos = 0, last_pipe = false;
        char c;
        while(1)
        {
            read(STDIN_FILENO, &c, 1);
            line[pos++] = c;
            if(last_pipe && c != ' ' && c != '\n')
                last_pipe = false;
            else if(c == '\n' && !isComplete(line, pos, last_pipe)){
                printf(">");
                fflush(NULL);
                continue;
            }
            else if(c == '\n'){
                line[pos - 1] = '\0';
                break;
            }
            else if(c == '|')
                last_pipe = true;
        }
        if(strlen(line) == 0)
            continue;
        int line_sz = 0;
        line = ReplaceInput(line, history, history_sz, &line_sz);
        if(line[0] != ' ')
            AddToHist(line, history, &history_sz, history_path);

        RunLine(line, history, history_sz, process1, process2);
        dup2(fdin, 0);
    }

    return 0;
}