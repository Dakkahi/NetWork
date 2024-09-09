#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define MAXLINE 1024

struct sockaddr_in addr;

void client_work(int acc_sock) {
    char buf[MAXLINE+1];
    memset(&buf, 0, sizeof(buf));

    int n = recv(acc_sock, buf, sizeof(buf)-1, 0);
    if(n > 0) {
        printf("%s", buf);
        
        memset(&buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf),
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<font color='blue'><h1>HELLO</h1></font>\r\n"
        "<font color='blue'><h2>This is the task of network computing</h2></font>\r\n");

        send(acc_sock, buf, strlen(buf), 0);
    }

    close(acc_sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    int uid = 1051;
    short int port = uid + 21560;
    int sock, acc_sock;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket");
        exit(1);
    }

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        exit(1);
    }

    if(listen(sock, 5) < 0) {
        perror("listen");
        close(sock);
        exit(1);
    }

    while(1) {
        acc_sock = accept(sock, (struct sockaddr *)NULL, NULL);
        if(acc_sock < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if(pid < 0) {
            perror("fork");
            close(acc_sock);
            continue;
        } else if(pid == 0) {
            close(sock);
            client_work(acc_sock);
        } else { 
            close(acc_sock);
        }
    }

    close(sock);
    return 0;
}
