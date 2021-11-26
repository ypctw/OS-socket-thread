#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>

#include "sock.h"
void Help()
{
    printf("-------------------------------------------------------------\n");
    printf("Connect\t\t\tDescription\n");
    printf("SET [key] [value]\tStore the K-V pair into the database.\n");
    printf("GET [key]\t\tGet value of [key] from database\n");
    printf("DELETE [key]\t\tDelete [key] from database\n");
    printf("EXIT\t\t\tEnd the Connection and Close\n");
    printf("-------------------------------------------------------------\n");
    return;
}
int main(int argc, char **argv)
{
    int opt;
    char *server_host_name = NULL, *server_port = NULL;

    /* Parsing args */
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host_name = malloc(strlen(optarg) + 1);
            strncpy(server_host_name, optarg, strlen(optarg));
            break;
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "[INFO] Unknown option \"-%c\"\n", isprint(optopt) ?
                    optopt : '#');
            return 0;
        }
    }

    if (!server_host_name)
    {
        fprintf(stderr, "[INFO] Error!, No host name provided!\n");
        exit(EXIT_FAILURE);
    }

    if (!server_port)
    {
        fprintf(stderr, "[INFO] Error!, No port number provided!\n");
        exit(EXIT_FAILURE);
    }

    /* Open a client socket fd */
    int clientfd __attribute__((unused)) = open_clientfd(server_host_name, server_port);
    /* Start your coding client code here! */
    if (0 >= clientfd)
    {
        printf("[INFO] Connect Error\n");
        close(clientfd);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Connet to %s : %s\n",server_host_name,server_port);
    char buf[100];
    char key[30],value[30];
    printf("[INFO] Welcome ! please type HELP for available Command\n> ");
    while(1)
    {
        fgets(buf, sizeof(buf), stdin);
        switch(buf[0])
        {
        case 'H':
            Help();
            break;
        case 'E':
            send(clientfd,"E",sizeof(buf),0);
            close(clientfd);
            exit(EXIT_SUCCESS);
        case 'S':
            sscanf(buf,"SET %s %s",key,value);
            while(send(clientfd,buf,sizeof(buf),0) < 0)
                printf("Retry!\n");
            while(recv(clientfd, buf, sizeof(buf), 0)<0)
                printf("Retry");
            if (buf[0] =='F')
                printf("[OK] PUT K-V pair (%s, %s) to DataBase!\n",key,value);
            break;
        case 'G':
            sscanf(buf,"GET %s",key);
            while(send(clientfd,buf,sizeof(buf),0) < 0)
                printf("Retry!\n");
            while(recv(clientfd, buf, sizeof(buf), 0) < 0)
                printf("Retry");
            if (buf[0] =='N')
                printf("[Fail] Cannot Find the Key !\n");
            else
            {
                sscanf(buf,"%s %s",key,value);
                printf("[OK] The Value of %s is %s ! !\n",key,value);
            }
            break;
        case 'D':
            sscanf(buf,"DELETE %s",key);
            while(send(clientfd,buf,sizeof(buf),0) < 0)
                printf("Retry!\n");
            while(recv(clientfd, buf, sizeof(buf), 0) < 0)
                printf("Retry");
            if (buf[0] =='F')
                printf("Finish Delete %s\n",key);
            break;
        default:
            continue;
        }
        printf("> ");
    }
    return 0;
}
