#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h> 
#include "trand.h"
using namespace std;

int count;

// initial client process including set up server address for socket connection
void client_init(sockaddr_in *serv_addr, char* argv[]){
    memset(serv_addr, '0', sizeof(*serv_addr));
    (*serv_addr).sin_family = AF_INET;
    (*serv_addr).sin_port = htons(atoi(argv[1]));
    (*serv_addr).sin_addr.s_addr =  inet_addr(argv[2]);
}

// header functinon gets the name of this client process(hostname.pid), sets up the output
// file and prints the header output for client log
void header(ofstream &outfile, char* id, char* argv[]){
    char file[100], hostname[1024];
    memset(hostname, '0', sizeof(hostname));
    int pid = getpid();                         // get process id
    gethostname(hostname, 1023);                // get host name
    snprintf(id, 100, "%s.%d", hostname, pid); 
    snprintf(file, 100, "%s.log", id);
    outfile.open(file, ios::out | ios::trunc);  // open output file
    // print header
    outfile << "Using port "<< argv[1]<<endl;
    outfile << "Using server address " << argv[2] <<endl;
    outfile << "Host " << id <<endl;
}

// client communication part
// For every input, the client first checks for 'S' or 'T' command
// if it's a sleep command, client performs Sleep() function.
// For every 'T' command, client first connects to the server,
// then it sends its id to the server and waits for respond.
// Finally, client sends out its command and waits for 'D' from server.
// After finishing sending one command, client closes the connection and waits for the next input.
void client_comm(sockaddr_in serv_addr, char* id, ofstream &outfile){
    char recvBuff[1024], sendBuff[1024], out[1024];
    double ms = 0;
    int sockfd = 0, n = 0;
    struct timeval tp;
    string temp;
    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));

    while(cin >> sendBuff){
        temp = sendBuff;
        if(sendBuff[0] == 'S'){
            // write log
            outfile << "Sleep "<< stoi(temp.substr(1), nullptr)<<" units"<<endl; 
            Sleep(stoi(temp.substr(1), nullptr));       // execute Sleep()
        }else{
            // connect to server
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

            write(sockfd, id, strlen(id));                  // send id to server
            read(sockfd, recvBuff, sizeof(recvBuff) - 1);   // wait for response
            write(sockfd, sendBuff, strlen(sendBuff));      // send data to server
            count++;                                        // count for transactions

            gettimeofday(&tp, NULL);
            ms = tp.tv_sec + (double)tp.tv_usec / 1000000;  // get time
            snprintf(out, 1024, "%.2f: Send (T%3d)", ms, stoi(temp.substr(1), nullptr));
            outfile << out << endl;                         // write log

            if((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0){     // write 'Done' transaction to log
                recvBuff[n] = 0;
                temp = recvBuff;
                gettimeofday(&tp, NULL);
                ms = tp.tv_sec + (double)tp.tv_usec / 1000000;
                snprintf(out, 1024, "%.2f: Recv (D%3d)", ms, stoi(temp.substr(1), nullptr));
                outfile << out<<endl;
            }
            
            close(sockfd);          // close the connection 

        }
    }
}

int main(int argc, char *argv[]){
    
    count = 0; 
    char id[100];
    struct sockaddr_in serv_addr;
    ofstream outfile;
    memset(id, '0', sizeof(id));
    
    client_init(&serv_addr, argv);          // initial client
    header(outfile, id, argv);              // print header
    client_comm(serv_addr, id, outfile);    // start to communicate
    
    outfile << "Sent " << count << " transactions" <<endl;
    outfile.close();


}