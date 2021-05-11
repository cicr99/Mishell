#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 8192

char* def_path;

void DefaultPath(char* home, char* pc_user){
    int home_sz = strlen(home), pc_sz = strlen(pc_user);
    def_path = malloc((home_sz + pc_sz + 5) * sizeof(char));
    int pos = 0;
    for(int i = 0; i < home_sz; ++i)
        def_path[pos++] = home[i];
    for (int i = 0; i < pc_sz; ++i)
        def_path[pos++] = pc_user[i];
    def_path[pos] = '\0';
}

//TODO si me pasan el path desde el ~
char *cd(char **argv, int argc)
{
    //TODO recoger errores
    if(argc == 1){
        chdir(def_path);
        return def_path;
    }

    char *path = argv[1];

    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);

    char *s;
    s = getcwd(buffer, BUFFERSIZE);

    int i = 0;
    while(1){
        if(s[i] == '\0')
            break;
        i++;
    }
    s[i] = '/';
    i++;
    int j = 0;

    while(1){
        if(path[j] == '\0')
            break;
        s[i] = path[j];
        i++;
        j++;
    }

    chdir(s);
    return s;
}
