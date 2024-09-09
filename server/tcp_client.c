#include<stdio.h>
#include<sys/types.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<error.h>

#define MAXLINE 1024

struct sockaddr_in server4;
struct sockaddr_in6 server6;


int main(int argc, char *argv[]){
    int sock, s, n;
    char deststr[MAXLINE+1];
    char buf[MAXLINE+1];

    strcpy(deststr, "52.199.137.81");
    int uid = 1051;
    short int destport = uid + 21560;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("socket");
        return 0;
    }

    server4.sin_family = AF_INET;
    server4.sin_port = htons(destport);
    s = inet_pton(AF_INET, deststr, &server4.sin_addr);
    if(s <= 0){
        perror("inet_pton");
        close(sock);
        return 0;
    }

    int c = connect(sock, (struct sockaddr *)&server4, sizeof(server4));
    if(c < 0){
        perror("connect");
        close(sock);
        exit(1);
    }

    char *request = "GET / HTTP/1.0\r\nHost: 52.199.137.81\r\n\r\n";
    write(sock, request, strlen(request));

    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));
    while(n > 0){
    printf("%s", buf);
    memset(buf, 0, sizeof(buf));
    }
}
