#ifndef H_HISTORY
#define H_HISTORY

char** init_history(char* path, int* length);
void AddToHist(char* input, char** history, int* length, char* path);
char* ReplaceInput(char* input, char** history, int history_sz, int* new_sz);
char* BuildPath(char* path, char* file_name);

#endif // H_HISTORY