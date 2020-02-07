#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process.h"

using namespace std;

struct process active_p [32];
int process_num;

// This function get and print user time and system time
void p_time(){
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);     // use getrusage() to get time
    float u_time = (usage.ru_utime.tv_sec + (float)usage.ru_utime.tv_usec / 1000000);
    float s_time = (usage.ru_stime.tv_sec + (float)usage.ru_stime.tv_usec / 1000000);
    printf("User time =    %d seconds\n", (int)u_time);
    printf("Sys  time =    %d seconds\n", (int)s_time);
}

// This function print the Running process table
void p_process(){
    int c = 0;
    for(int i = 0; i < 32; i++){
        if(active_p[i].state != NULL){
            printf("%d:  %5d  %c  %3d  %s",c , active_p[i].pid, active_p[i].state, get_time(active_p[i].pid), active_p[i].cmd);
            c++;
        }
    }
}

// Since the struct array has a size of 32, when the array is full,
// we create a new array and put all active process in, and then
// copy the new array to the old one.
void check_array(){
    if(process_num == 32){
        struct process temp[32];
        int c = 0;
        for(int i = 0; i < 32; i++){
            if(active_p[i].state != NULL){
                temp[c].pid = active_p[i].pid;
                strcpy(temp[c].cmd, active_p[i].cmd);
                temp[c].state = active_p[i].state;
                c++;
            }
        }
        copy(temp, temp + 32, active_p);
    }
}

// get_time() function uses pipe and execute "ps" command to get the 
// real time cpu time for the Running process table.
// It returns the cpu time of a process in seconds.
int get_time(pid_t pid){
    char* cmd[10];
    char* target[10];
    char buf[100];
    int n, fd[2];
    // set up the ps command
    cmd[0] = (char*)"ps";
    cmd[1] = (char*)"-p";
    cmd[2] = new char[10];
    snprintf(cmd[2], 10, "%d", pid);
    cmd[3] = (char*)"-o";
    cmd[4] = (char*)"time";
    cmd[5] = (char*)"--no-header";
    cmd[6] = NULL;

    if (pipe(fd) < 0) perror("pipe error!");   // create pipe
    int rc = fork();
    signal(SIGCHLD, SIG_IGN);
    if(!rc){            // child process
        close(fd[0]);   
        dup2(fd[1], STDOUT_FILENO);     // link the pipe to stdout
        close(fd[1]);
        execvp(cmd[0], cmd);            // execute ps command to get time
    }else if(rc > 0){   // parent process
        close(fd[1]);                   
        n = read(fd[0], buf, 100);      // read the content send from pipe(the output of ps command)
        close(fd[0]);
    }
    split_colon(buf, target);           // split the time(00:00:00) into sec
    int time = 0;
    time = atoi(target[0]) * 3600 + atoi(target[1]) * 60 + atoi(target[2]); // calculate time
    delete[] cmd[2];
    return time;
}

// split colon 
void split_colon(char* cmd, char** t){
    int count = 0;
    t[count] = strtok(cmd, ":");
    while(t[count] != NULL){
        count++;
        t[count] = strtok(NULL, ":");
    }

}

// delete child information from process table after it is terminated
void clean_child(pid_t pid){
    bool s = false;
    for(int i = 0; i < 32; i++){
        if(active_p[i].pid == pid){
            active_p[i].state = NULL;
            s = true;
        }
    }
    if(s) {
        process_num--;
    }
}

// check the given pid is running or not
bool check_child(pid_t pid){
    for(int i = 0; i < 32; i++){
        if(active_p[i].pid == pid){
            return true;
        }
    }
    return false;
}

// get the state for a given pid
// return "" if pid not exist
char child_state(pid_t pid){
    for(int i = 0; i < 32; i++){
        if(active_p[i].pid == pid){
            return active_p[i].state;
        }
    }
    return NULL;
}

// change process state
void change_state(pid_t pid, char state){
    for(int i = 0; i < 32; i++){
        if(active_p[i].pid == pid){
            active_p[i].state = state;
            return;
        }
    }
    printf("process doesn't exist\n");
}