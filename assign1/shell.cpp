#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process.h"
#include "cmd_line.h"

using namespace std; 


// sig_child(int) function is used to catch when process running in the
// background exits.
void sig_child(int signo)
{
    pid_t pid;
    while ((pid = waitpid(-1,NULL,WNOHANG)) > 0){
        clean_child(pid);                   // delete the process from the "Running process" table
    } 
}

// special function used to execute process in the background
void exec_back(char** target, char* cmd, int length){
    
    pid_t rc1 = fork();
    if(!rc1){           // child process
        
        det_brackets(target, length);       // determine whether the command needs to redirect input(output) from(to) a file
        execvp(target[0], target);
        printf("can't execute command\n");  // print error message if can't execute execvp()
        exit(0);                            // kill child process
    }else if(rc1 > 0){  // parent process
        // save process information
        active_p[process_num].pid = rc1;    
        strcpy(active_p[process_num].cmd,cmd);
        active_p[process_num].state = 'R';
        process_num++;

        check_array();                      // check if the process array is full
    }else{
        perror("fork error");               // fork error
        //exit(1);
    }
} 

// used to handle command line contents
void exe_cmd(char* cmd){
    // split the command line by space
    char copy[100];
    char* target[10];
    strcpy(copy, cmd);
    int length = split_cmd(copy, target);

    if(length > 0 && det_and(target, length)){      // check if the command is not empty and if there is a '&' in the end

        exec_back(target, cmd, length);             // execute the program in the background
    }else if(target[0] != NULL){
        // execute in the foreground
        if(det_cmd(target)){                        // determine if the command is special command
            pid_t rc = fork();
            if(!rc){            // child process
                det_brackets(target, length);       // determine whether the command needs to redirect input(output) from(to) a file
                execvp(target[0], target);
                printf("can't execute command\n");  // error message
                exit(0);                            // kill child
            }else if(rc > 0){   // parent process
                pid_t rcc = wait(NULL);             // wait for child to complete
            }else{
                perror("fork error");
                //exit(1);
            }
        }
    }
} 


int main(){
    char* cmd = NULL;                   // command line input
    size_t s = 0;
    process_num = 0;                    
    while(true){  
        signal(SIGCHLD, sig_child);     // set signal to catch when the child process running in the background exits
        printf("SHELL379: ");           // promt
        getline(&cmd, &s, stdin);       // get input line
        exe_cmd(cmd);                   // execute command 
    }
    return 0;
}