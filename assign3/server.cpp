#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
using namespace std;



int main(int argc, char *argv[]){

    struct sockaddr_in serv_addr; 
    time_t ticks;
    char recvBuff[1024];
    char sendBuff[1024];
    int serverfd = 0, connfd = 0;
    int n = 0;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(recvBuff, '0', sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));

    snprintf(sendBuff, sizeof(sendBuff), "done");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5100);

    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(serverfd, 10); 
    
    while(true){
        //cout <<"1"<<endl;
        connfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
        cout <<"connected"<<endl;
        if((n = read(connfd, recvBuff, sizeof(recvBuff) - 1)) > 0){
            cout<< "read"<<endl;
            recvBuff[n] = 0;
            cout << recvBuff <<endl;
            write(connfd, sendBuff, sizeof(sendBuff) - 1);
        }
        close(connfd);
        //  sleep(1);
    }
}






