#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <queue>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include"trand.h"
using namespace std;
using namespace chrono;

extern time_point<high_resolution_clock> start;
extern queue<string> cmd;
extern sem_t s1,s2;
extern bool s;
extern int work, ask, receive, slp;
extern ofstream outfile;


// this function is to calculate the time duration from the program started to
// the time this function is called
double get_time(){
    auto end = high_resolution_clock::now();
    duration<double> t = end - start;
    return (double)(t.count());
}

// producer thread function
void* producer(void* arg){

    string temp;
    char out[100];
    int size = 0;
    work = 0;
    slp = 0;
    
    while(cin >> temp){                 // read input

        if(temp.at(0) == 'S'){         // if it's a sleep command, execute directly
            slp++;
            // semaphore s2 is used to control file writing, making sure there's only one
            // thread writing to file
            sem_wait(&s2);              
            sprintf(out, "   %.3f ID=0     Sleep       %d", get_time(), stoi(temp.substr(1), nullptr));
            outfile << out <<endl;
            sem_post(&s2);

            Sleep(stoi(temp.substr(1), nullptr));   // execute Sleep() function
            continue;                               // jump directly to the next loop
        }
        while(cmd.size() >= 2 * (*(int*)arg)){      // if the queue is full, wait
            usleep(0);
        }
        if(temp != ""){                             // execute T<> command
            // semaphore s1 is used to as mutual exclusion for writing and reading queue,
            // making sure the consumer won't pop the wrong command
            sem_wait(&s1);
            cmd.push(temp);
            size = cmd.size();
            sem_post(&s1);

            work++;                     // work count +1

            sem_wait(&s2);
            sprintf(out, "   %.3f ID=0 Q=%d Work        %d", get_time(), size, stoi(temp.substr(1), nullptr));
            outfile << out <<endl;
            sem_post(&s2);
            
        }
        //outfile.flush();
    }
    
    s = true;           // when producer finish reading, set s to true to let consumer know
    return((void *)0);
}

// consumer thread function
void* consumer(void* a){
    string temp;
    char out[100];
    int size = 0;
    int complete = 0;
    ask++;              // ask count +1

    sem_wait(&s2);
    sprintf(out, "   %.3f ID=%d     Ask", get_time(), *(int*)a);
    outfile << out <<endl;
    sem_post(&s2);

    while(true){
        while(!cmd.empty()){        // if the queue is empty, wait

            sem_wait(&s1);
            temp = cmd.front();     // get command from queue
            if(!cmd.empty()){       
                cmd.pop();          // pop the command
            }
            size = cmd.size();
            sem_post(&s1);

            if(temp != ""){
                receive++;          // receive count +1

                sem_wait(&s2);
                sprintf(out, "   %.3f ID=%d Q=%d Receive     %d", get_time(), *(int*)a, size, stoi(temp.substr(1), nullptr));
                outfile << out <<endl;
                sem_post(&s2);

                Trans(stoi(temp.substr(1), nullptr));       // execute Trans() function
                complete++;                                 // complete count +1

                sem_wait(&s2);
                sprintf(out, "   %.3f ID=%d     Complete    %d", get_time(), *(int*)a, stoi(temp.substr(1), nullptr));
                outfile << out <<endl;
                sem_post(&s2);
            }
            ask++;                  // ask count +1

            sem_wait(&s2);
            sprintf(out, "   %.3f ID=%d     Ask", get_time(), *(int*)a);
            outfile << out <<endl;
            sem_post(&s2);
        }

        usleep(0);
        if(s && cmd.empty()){       // if the queue is empty and producer finishes reading, jobs done, return

            *(int*)a = complete;    // use the argument pointer passed in to save complete count and pass out to main function
            return((void*)a);
        }
    }
}
