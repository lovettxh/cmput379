#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unordered_map>
#include <sys/types.h>
#include <sys/time.h> 
#include "trand.h"
using namespace std;

int count;
unordered_map<string, int> map;
double b = 0, e = 0;
bool s = 1;
// server commumication part
// When the server starts, it will wait for client's connection.
// And when it is connected, the server waits for client to send its id
// and responds to client. Then the server waits for command to be sent over
// and execute the function.
void server_comm(int serverfd, struct sockaddr_in serv_addr, ofstream &outfile){
    char client[100], recvBuff[1024], sendBuff[1024], out[1024];
    int n = 0, connfd = 0;
    string temp;
    string c;
    struct timeval tp;
    double ms;
    memset(client, '0', sizeof(client));
    memset(recvBuff, '0', sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));
    // waits for client's connection
    connfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
    // read client's id
    if((n = read(connfd, client, sizeof(client) - 1)) > 0){
        client[n] = 0;
        c = client;
    }
    snprintf(sendBuff, sizeof(sendBuff), "R");
    // sending confirmation message
    write(connfd, sendBuff, sizeof(sendBuff) - 1);
    // read command
    if((n = read(connfd, recvBuff, sizeof(recvBuff) - 1)) > 0){
        recvBuff[n] = 0;
        temp = recvBuff;
        if(recvBuff[0] == 'T'){
            count++;
            if(map.count(c) > 0){           // unordered_map is used to track how many transactions each client sends
                map[c]++;               
            }else{
                map.insert(make_pair(c, 1));
            }
            gettimeofday(&tp, NULL);
            ms = tp.tv_sec + (double)tp.tv_usec / 1000000;
            if(s){                          // collect time when the transactions start
                b = ms;
                s = 0;
            }

            snprintf(out, 1024, "%.2f: #  %d (T%3d) from %s", ms, count, stoi(temp.substr(1), nullptr), client);
            outfile << out << endl;                             // write to log file for receiving command
            Trans(stoi(temp.substr(1), nullptr));               // execute command
            gettimeofday(&tp, NULL);
            ms = tp.tv_sec + (double)tp.tv_usec / 1000000;
            snprintf(sendBuff, sizeof(sendBuff), "D%d", count);
            write(connfd, sendBuff, sizeof(sendBuff) - 1);      // send done message to client plus the number of the transaction
            snprintf(out, 1024, "%.2f: #  %d (Done) from %s", ms, count, client);
            outfile << out << endl;                             // write to log file for finishing executing command
            e = ms;                                             // collect transaction end time
        }
          
    }
    close(connfd);                                              // close connection after finishing one transaction
    
}

// server initial function
// It sets up the socket, serv_addr and binds them together
void server_init(int *serverfd, struct sockaddr_in *serv_addr, char *argv[]){
    memset(serv_addr, '0', sizeof(*serv_addr));
    *serverfd = socket(AF_INET, SOCK_STREAM, 0);
    (*serv_addr).sin_family = AF_INET;
    (*serv_addr).sin_addr.s_addr = htonl(INADDR_ANY);
    (*serv_addr).sin_port = htons(atoi(argv[1]));
    bind(*serverfd, (struct sockaddr*)serv_addr, sizeof(*serv_addr)); 
    listen(*serverfd, 100); 
}

// Output the summary part at the end of the logfile
void summary(ofstream &outfile){
    outfile << "SUMMARY" << endl;
    for(auto& i : map){
        outfile << "  " << i.second << " transactions from " << i.first << endl;
    }
    char out[1024];
    snprintf(out, 1024, "%.1f transactions/sec (%d/%.2f)", count/(e - b), count, (e - b));
    outfile << out << endl;
}


int main(int argc, char *argv[]){
    count = 0;
    struct sockaddr_in serv_addr; 
    int serverfd = 0;
    struct timeval timeout,tp;
    fd_set s;
    ofstream outfile;

    server_init(&serverfd, &serv_addr, argv);
    outfile.open("server.log", ios::out | ios::trunc);
    outfile << "Using port" << argv[1] <<endl;

    while(true){                                                    // a loop used for reading input from client
        // set up timeout for waiting for connections
        FD_ZERO(&s);
        FD_SET(serverfd, &s);
        timeout = {60,0};
        if(select(serverfd + 1, &s, NULL, NULL, &timeout) == 0){    // when the time is up, server exits
            cout << "timeout" << endl;
            close(serverfd);
            summary(outfile);
            outfile.close();
            return 0;
        }
        server_comm(serverfd, serv_addr, outfile);                  // communicate to client
        
    }
    
}






