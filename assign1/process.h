#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

struct process
{
    pid_t pid;
    char cmd[100];
    char state;
};

extern struct process active_p [32];
extern int process_num;


void p_time();
void p_process();

void check_array();
int get_time(pid_t pid);

void split_colon(char* cmd, char** t);

void clean_child(pid_t pid);
bool check_child(pid_t pid);
char child_state(pid_t pid);
void change_state(pid_t pid, char state);