#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

void signalHandler(int signo)
{
    // su ly su kien tien trinh con ket thuc
    pid_t pid = wait(NULL);
    printf("Child process terminated, pid = %d.\n", pid);
}

int main()
{
    // Tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    { // tao socket that bai
        perror("Failed to create socket.");
        exit(1);
    }

    // khai bao cau truc dia chi server va gan vao socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long: little-endian ---> big-endian cho so nguyen dai
    addr.sin_port = htons(9001);              // host to network short: little-endian ---> big-endian cho so nguyen ngan

    // Gan dia chi voi socket
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("failed to bind.");
        exit(1);
    }

    // cho ket noi
    if (listen(listener, 5))
    {
        perror("failed to listen.");
        exit(1);
    }

    char buf[256];
    for(int i = 0; i < 10; i++) {
        if (fork() == 0) {
            //tien trinh con chay nhu 1 server
            while (1) {
                int client = accept(listener, NULL, NULL);
                printf("New client connect: %d\n", client);

                //nhan du lieu tu client 
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0) {
                    continue;
                }
                buf[ret] = 0;
                printf("Received from %d: %s\n", client, buf);
                // Trả lại kết quả cho client
                char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
                send(client, msg, strlen(msg), 0);
                close(client);
            }

            exit(0);
        }
    }

        // dong client socket o tien trinh cha
        getchar();
        killpg(0, SIGKILL);
}