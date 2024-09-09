#include<stdio.h>
#include<sys/types.h>
#include <stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<error.h>

#define MAXLINE 1024

struct sockaddr_in server4;

struct sockaddr_in6 server6;

bool isIPv4(const char *ipaddress){
    int flag = 0;
    for(int i = 0; ipaddress[i] != '\0'; i++){
        if(ipaddress[i] == '.'){
            flag = 1;
            break;
        }
    }
    return (flag == 1);
}

int main(int argc, char *argv[]){

    char *ipaddress = argv[1];
    char deststr[MAXLINE+1];
    char buf[MAXLINE+1];

    strcpy(deststr, argv[1]);
    short int destport = 13;
    int sock;

    //IPv4の場合
    if(isIPv4(ipaddress)){

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(socket < 0){
            perror("socket");
            return 0;
        }

        server4.sin_family = AF_INET;
        server4.sin_port = htons(destport);
        int s = inet_pton(AF_INET, deststr, &server4.sin_addr);
        if(s < 1){
            perror("inet_pton");
            return 0;
        }

        connect(sock, (struct sockaddr *)&server4, sizeof(server4));
        if(connect < 0){
            perror("connect");
            return 0;
        }
    }else{
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        if(sock < 0){
            perror("socket");
            return 0;
        }

        server6.sin6_family = AF_INET6;
        server6.sin6_port = htons(destport);
        int s = inet_pton(AF_INET6, deststr, &server6.sin6_addr);
        if(s < 1){
            perror("inet_pton");
            return 0;
        }

        connect(sock, (struct sockaddr *)&server6, sizeof(server6));
        if(connect < 0){
            perror("connect");
            return 0;
        }


    }

    memset(buf, 0, sizeof(buf));
    int n = read(sock, buf, sizeof(buf));
    if(n < 0){
        perror("read");
        return 0;
    }

    printf("%s\n", buf);

    close(sock);
    return 0;
}