#ifndef PROCESS_H_INCLUDED
#define PROCESS_H_INCLUDED
#define MAX_PID_NUM     32  
char *basename(const char *path);
int get_pid_by_name(const char* process_name, pid_t pid_list[], int list_size);
int is_process_exist(const char* process_name);
#endif // PROCESS_H_INCLUDE
