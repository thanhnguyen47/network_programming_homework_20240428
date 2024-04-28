#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <time.h>

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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);         

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

    signal(SIGCHLD, signalHandler); 
    while (1)
    {
        printf("waiting for new client.\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);

        if (fork() == 0)
        {
            // tien trinh con xu ly yeu cau cua client

            // dong socket listener o tien trinh con
            // close(listener);

            char buf[256];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    continue;

                buf[ret] = 0;
                printf("received: %s", buf);
                char label[10], format[20], tmp[20];
                int n = sscanf(buf, "%s %s %s", label, format, tmp);
                if (n != 2 || strcmp(label, "GET_TIME"))
                {
                    char *msg = "Error Format.\n";
                    send(client, msg, strlen(msg), 0);
                }
                else
                {
                    if (strcmp(format, "dd/mm/yyyy") == 0)
                    {
                        time_t t = time(NULL);
                        struct tm date = *localtime(&t);

                        char msg[100];
                        sprintf(msg, "Current date is %02d/%02d/%d\n", date.tm_mday, date.tm_mon + 1, date.tm_year + 1900);
                        send(client, msg, strlen(msg), 0);
                    }
                    else if (strcmp(format, "dd/mm/yy") == 0)
                    {
                        time_t t = time(NULL);
                        struct tm date = *localtime(&t);

                        char msg[100];
                        sprintf(msg, "Current date is %02d/%02d/%02d\n", date.tm_mday, date.tm_mon + 1, (date.tm_year + 1900)%100);
                        send(client, msg, strlen(msg), 0);
                    }
                    else if (strcmp(format, "mm/dd/yyyy") == 0)
                    {
                        time_t t = time(NULL);
                        struct tm date = *localtime(&t);

                        char msg[100];
                        sprintf(msg, "Current date is %02d/%02d/%d\n", date.tm_mon + 1, date.tm_mday, date.tm_year + 1900);
                        send(client, msg, strlen(msg), 0);
                    }
                    else if (strcmp(format, "mm/dd/yy") == 0)
                    {
                        time_t t = time(NULL);
                        struct tm date = *localtime(&t);

                        char msg[100];
                        sprintf(msg, "Current date is %02d/%02d/%02d\n", date.tm_mon + 1, date.tm_mday, (date.tm_year + 1900)%100);
                        send(client, msg, strlen(msg), 0);
                    }
                    else
                    {
                        char *msg = "Error Format.\n";
                        send(client, msg, strlen(msg), 0);
                    }
                }
                // trả lại kết quả cho client
            }

            // ket thuc tien trinh con
            exit(0);
        }

        // dong client socket o tien trinh cha
        close(client);
    }
}