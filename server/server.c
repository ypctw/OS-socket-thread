#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <stdbool.h>
#include "types.h"
#include "sock.h"
typedef struct HashNode
{
    struct HashNode* next;
    char *key;
    char *value;
} HashNode;
int Hashing (char* Key);
HashNode* HashNode_Create(char *Key, char *Value);
HashNode* HashNode_Insert(HashNode* Root, char* Key,char* Value);
HashNode* HashNode_Search(HashNode* Root,char *Key);
HashNode* HashNode_Delete(HashNode* Root,char *Key);
HashNode* HashTable[26];
bool *thread_sum;
void *child(void *arg)
{
    printf("Into Thread\n");
    int* data = (int*)arg;
    char buf[100],key[30],value[30];
    int connect_fd = data[0];
    int thread_num = data[1];
    while(1)
    {
        if (0 > recv(connect_fd, buf, sizeof(buf), 0))
            printf("fail");
        else
        {
            printf("%s",buf);
            switch(buf[0])
            {
            case 'S':
                sscanf(buf,"SET %s %s",key,value);
                HashTable[Hashing(key)] = HashNode_Insert(HashTable[Hashing(key)],key,value);
                strcpy(buf,"Finish");
                while(send(connect_fd,buf,sizeof(buf),0) < 0)
                    printf("Retry\n");
                break;
            case 'G':
                sscanf(buf,"GET %s",key);
                HashNode* node = HashNode_Search(HashTable[Hashing(key)],key);
                if(node == NULL)
                    strcpy(buf,"NoFound");
                else
                {
                    strcpy(buf, "");
                    memset(buf, '\0', strlen(node->value)+strlen(node->key)+1);

                    strcat(buf,node->key);
                    strcat(buf," ");
                    strcat(buf,node->value);
                }
                while(send(connect_fd,buf,sizeof(buf),0) < 0)
                    printf("Retry\n");
                break;
            case 'D':
                sscanf(buf,"DELETE %s",key);
                HashTable[Hashing(key)] = HashNode_Delete(HashTable[Hashing(key)],key);
                strcpy(buf,"Finish");
                while(send(connect_fd,buf,sizeof(buf),0) < 0)
                    printf("Retry\n");
                break;
            case 'E':
                thread_sum[thread_num] = false;
                close(connect_fd);
                pthread_exit(NULL);
                break;
            }
        }
    }
}

int main(int argc, char **argv)
{
    //Create Hash Table

    for (int i = 0 ; i<26; ++i)
    {
        HashTable[i] = (HashNode*)malloc(sizeof(HashNode));
        HashTable[i]->next = NULL;
    }
    char *server_port = 0;
    int opt = 0;
    /* Parsing args */
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ?
                    optopt : '#');
            return 0;
        }
    }

    if (!server_port)
    {
        fprintf(stderr, "Error! No port number provided!\n");
        exit(1);
    }
    /* Open a listen socket fd */
    int listenfd __attribute__((unused)) = open_listenfd(server_port);
    /* Start coding your server code here! */
    struct sockaddr_storage clientaddr;
    socklen_t client_length = sizeof(struct sockaddr_storage);
    pthread_t threads[10];
    thread_sum = (bool*)malloc(10*sizeof(bool));
    for (int i = 0; i<10; ++i)
        thread_sum[i] = false;
    while(1)
    {
        int now_thread = 0;
        while(1)
        {
            if(thread_sum[now_thread]==false)
                break;
            now_thread++;
        }
        printf("now_thread %d\n",now_thread);
        int connect_fd  = accept(listenfd,(SA*) &clientaddr,&client_length);
        int data[2];
        data[0] = connect_fd;
        data[1] = now_thread;
        if (now_thread==10)
            printf("server is full\n");
        else
        {
            pthread_create(&threads[now_thread],NULL,child,(void*)data);
            thread_sum[now_thread]= true;
        }
    }
    return 0;
}
int Hashing(char* Key)
{
    int hash = 0;
    for(int i = 0 ; i<strlen(Key); ++i)
        hash += (int)Key[i] - (int)'a';
    hash %= 26;
    hash += 26;
    return hash % 26;
}

HashNode* HashNode_Create(char *Key, char *Value)
{
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    node->next = NULL;
    node->key = (char*) malloc(sizeof(Key));
    strcpy(node->key,Key);
    node->value = (char*) malloc(sizeof(Value));
    strcpy(node->value,Value);
    return node;
}
HashNode* HashNode_Insert(HashNode* Root, char* Key,char* Value)
{

    if(Root == NULL)
        Root->next = HashNode_Create(Key,Value);
    else
    {
        HashNode* temp = Root;
        while(temp->next!=NULL)
        {
            temp = temp->next;
            if(strcmp(temp->key,Key) == 0)
            {
                temp->value = Value;
                return Root;
            }
        }
        temp->next = HashNode_Create(Key,Value);
    }

    return Root;
}
HashNode* HashNode_Search(HashNode* Root,char *Key)
{
    if(Root == NULL)
        return (HashNode*)NULL;
    else
    {
        HashNode* temp = Root->next;
        while(temp!=NULL)
        {
            if(strcmp(temp->key,Key) == 0)
                return temp;
            temp = temp->next;
        }
    }
    return (HashNode*)NULL;
}
HashNode* HashNode_Delete(HashNode* Root, char* Key)
{
    HashNode* temp = Root->next;
    if(Root == NULL)
        return (HashNode*)NULL;
    else if (strcmp(temp->key,Key) == 0)
    {
        Root->next = temp->next;
        free(temp);
    }
    else
    {
        HashNode* temp_last = temp;
        temp = temp->next;
        while(temp!=NULL)
        {
            if(strcmp(temp->key,Key) == 0)
            {
                temp_last->next = temp->next;
                free(temp);
                break;
            }
            else
            {
                temp_last = temp;
                temp = temp->next;
            }
        }
    }
    return Root;
}