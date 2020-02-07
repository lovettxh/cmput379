#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

#include "process.h"

using namespace std;

// split command line input by space and \n
int split_cmd(char* cmd, char** t){
    int count = 0;
    t[count] = strtok(cmd," \n");
    while(t[count] != NULL){
        count++;
        t[count] = strtok(NULL," \n");
    }
    return count;
}

// determine whether the command is a special command
// return 0(false) if the command is a special command, return 1(true) if it isn't
bool det_cmd(char** target){
    // "jobs" prints Running process table, number of the active process and
    // user time and system time used for the completed processes
    if(strcmp(target[0], "jobs") == 0){
        printf("Running processes:\n");
        printf("#     PID  S  SEC  COMMAND\n");
        p_process();
        printf("Processes = %d active\n", process_num);
        printf("Completed processes:\n");
        p_time();
        return 0;
        
    // "sleep" command makes shell to sleep for <int> seconds
    }else if(strcmp(target[0], "sleep") == 0){
        usleep(atoi(target[1]) * 1000000);
        return 0;

    // "exit" command waits for all running process to complete and exit shell
    }else if(strcmp(target[0], "exit") == 0){
        for(int i = 0; i < 32; i++){
            if(active_p[i].state == 'S'){
                kill(active_p[i].pid, 9);
            }
        }
        while(wait(NULL) > 0);
        printf("Resources used\n");
        p_time();
        exit(0);
        return 0;

    // "kill" command terminate process given its pid
    }else if(strcmp(target[0], "kill") == 0){
        if(check_child(atoi(target[1]))){
            kill(atoi(target[1]), 9);
        }else{
            printf("process doesn't exist\n");  // error message if the pid does not exist
        }
        return 0;

    // "wait" command makes the shell to wait for specific shell to complete running
    }else if(strcmp(target[0], "wait") == 0){
        if(check_child(atoi(target[1]))){                       // check if the process exists
            if(child_state(atoi(target[1])) == 'R'){            // check the pid is in Running mode
                pid_t pid;
                pid = waitpid(atoi(target[1]), NULL, 0);
                clean_child(pid);
            }else{
                printf("can't wait for suspend process\n");     // error message
            }
        }else{
            printf("process doesn't exist\n");                  // error message
        }
        return 0;

    // "suspend" command stops the process given its pid
    }else if(strcmp(target[0], "suspend") == 0){
        if(check_child(atoi(target[1]))){                       // check if the process exists
            kill(atoi(target[1]), 19);
            change_state(atoi(target[1]), 'S');                 // change process state
        }else{
            printf("process doesn't exist\n");                  // error message
        }
        return 0;

    // "resume" command allows the process to start running
    }else if(strcmp(target[0], "resume") == 0){
        if(check_child(atoi(target[1]))){                       // check if the process exists
            kill(atoi(target[1]), 18);
            change_state(atoi(target[1]), 'R');                 // change process state
        }else{
            printf("process doesn't exist\n");                  // error message
        }
        return 0;
    }
    return 1;
}

// This function determine whether the command follows a '&'
// which indicates running in the background
bool det_and(char** target, int length){
    if(strcmp(target[length - 1], "&") == 0){
        return 1;
    }
    return 0;
}

// This function tests input line to see if there are redirect of input and output to file
void det_brackets(char** target, int length){
    char* t[2];
    int file;
    for(int i = 0; i < length; i++){
        if(target[i][0] == '<'){
            t[0] = strtok(target[i], "<");
            file = open(t[0], O_RDONLY, 0644);
            dup2(file, STDIN_FILENO);
            close(file);
            target[i] = "";
        }else if(target[i][0] == '>'){
            t[0] = strtok(target[i], ">");
            file = open(t[0], O_WRONLY | O_CREAT, 0644);
            dup2(file, STDOUT_FILENO);
            close(file);
            target[i] = "";
        }
    }
}