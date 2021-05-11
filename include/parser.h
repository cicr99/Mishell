#ifndef H_PARSER
#define H_PARSER

void RunLine(char* input, char** history, int history_sz, int *process1, int *process2);
int isComplete(char *line, int sz, int last_pipe);

#endif // H_PARSER