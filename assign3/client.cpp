#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
using namespace std;

int main(int argc, char *argv[]){
    int sockfd = 0, n = 0;
    char recvBuff[1025];
    char sendBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
    

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5100);
    serv_addr.sin_addr.s_addr =  inet_addr(argv[1]);
    

    
    while(true){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        

        cin >> sendBuff;
        write(sockfd, sendBuff, strlen(sendBuff)); 

        if((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0){
            recvBuff[n] = 0;
            cout << recvBuff <<endl;
            
        } 
        close(sockfd);
        //sleep(1);
    
    }

}