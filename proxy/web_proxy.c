#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>

#define MAXSIZE 4096

struct sockaddr_in addr;

void client_work(int acc_sock) {
    char buf[MAXSIZE+1];
    char url[MAXSIZE+1];
    char GET[MAXSIZE+1];
    char version[MAXSIZE+1];
    char hostname[MAXSIZE+1];
    char path[MAXSIZE+1];
    memset(&buf, 0, sizeof(buf));
    int n;
    int server_sock;

    n = recv(acc_sock, buf, sizeof(buf)-1, 0);
    if (n <= 0) {
        close(acc_sock);
        return;
    }

    printf("Received request:\n%s\n", buf);

    sscanf(buf, "%s %s %s", GET, url, version);

    sscanf(url, "http://%99[^/]%199[^\n]", hostname, path);

    struct addrinfo hints, *res, *res0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    if (getaddrinfo(hostname, "http", &hints, &res0) != 0) {
        close(acc_sock);
        return;
    }

    res = res0;
    server_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_sock < 0) {
        freeaddrinfo(res0);
        close(acc_sock);
        return;
    }

    if (connect(server_sock, res->ai_addr, res->ai_addrlen) < 0) {
        close(server_sock);
        freeaddrinfo(res0);
        close(acc_sock);
        return;
    }

    freeaddrinfo(res0);

    snprintf(buf, sizeof(buf), "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, hostname); 
    send(server_sock, buf, strlen(buf), 0);

    while ((n = recv(server_sock, buf, sizeof(buf)-1, 0)) > 0) {
        send(acc_sock, buf, n, 0);
    }

    close(server_sock);
    close(acc_sock);
}

int main(int argc, char *argv[]) {
    int uid = 1051;
    short int port = uid + 21560;
    int sock, acc_sock;
    struct addrinfo hints, *res, *res0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //任意のアドレスをバインドできるようにする

    char port_str[6]; //ポート番号を文字列に変換(getaddrinfoで使うため)
    snprintf(port_str, sizeof(port_str), "%d", port);

    if ((getaddrinfo(NULL, port_str, &hints, &res)) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    res0 = res;
    while (res0 != NULL) {
        sock = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);
        if (sock == -1) {
            res0 = res0->ai_next;
            continue;
        }

        if (bind(sock, res0->ai_addr, res0->ai_addrlen) == -1) {
            close(sock);
            res0 = res0->ai_next;
            continue;
        }
        break;
        res0 = res0->ai_next;
    }

    if (res0 == NULL) {
        freeaddrinfo(res);
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(sock, 5) < 0) {
        perror("listen");
        close(sock);
        exit(1);
    }

    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        acc_sock = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (acc_sock < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(acc_sock);
            continue;
        } else if (pid == 0) {
            close(sock);
            client_work(acc_sock);
            exit(0);
        } else { 
            close(acc_sock);
        }
    }

    close(sock);
    return 0;
}
