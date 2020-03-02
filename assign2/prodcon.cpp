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
#include "thread_function.h"
#include "trand.h"
using namespace std;
using namespace chrono;

time_point<high_resolution_clock> start;
queue<string> cmd;
sem_t s1,s2;
bool s;
int work, ask, receive, slp;
ofstream outfile;



int main(int argc, char* argv[]){
    start = high_resolution_clock::now();   // record start time 
    int a = atoi(argv[1]);                  // argument for number of comsumer threads

    sem_init(&s1, 0, 1);                    // initiate semaphores       
    sem_init(&s2, 0, 1);
    int count = 0;
    
    char file[100];
    ask = 0;
    receive = 0;

    s = false;                              // a switch for determining whether producer has read all the inputs

    int* arg = new int[a + 1];
    pthread_t* tid = new pthread_t[a + 1];

    if(argc == 2 || (argc == 3 && strcmp(argv[2], "0") == 0)){  // determine whether id equals to 0 and open the output file
        sprintf(file, "prodcon.log");
        outfile.open(file, ios::out | ios::trunc );
    }else{
        sprintf(file, "prodcon.%s.log", argv[2]);
        outfile.open(file, ios::out | ios::trunc );
    }
    
    while(count < a){                       // create consumer threads with the given number
        arg[count] = count + 1;             // the argument passes to thread function to identify each thread
        if( pthread_create( &tid[count + 1], NULL, consumer, (void *)&arg[count] ) ) {
            perror( "Thread create2" );
            return( -1 );
        }
        count++;
    }
    if( pthread_create( &tid[0], NULL, producer, (void *)&a ) ) {   // create producer thread with the argument that
                                                                    // specifies the queue size
            perror( "Thread create1" );
            return( -1 );
    }

    
    int complete = 0;                                               
    for(int i = 0; i < a; i++){                                     //  waiting for all the consumer threads to complete
        pthread_join(tid[i + 1], nullptr);
        complete += arg[i];                                         // count total completed jobs

    }
    // Summary part
    sprintf(file, "Transactions per second: %.2f", complete / get_time());
    outfile << "Summary" << endl;
    outfile << "  Work          " << work << endl;
    outfile << "  Ask           " << ask << endl;
    outfile << "  Receive       " << receive << endl;
    outfile << "  Complete      " << complete << endl;
    outfile << "  Sleep         " << slp << endl;
    for(int i = 0; i < a; i++){
        outfile << "  Thread " << i + 1 << "      " << arg[i] <<endl;
    }
    outfile << file << endl;
    outfile.close();                    // close file
    // free the dynamic array
    delete [] tid;
    delete [] arg;
    return 0;

}