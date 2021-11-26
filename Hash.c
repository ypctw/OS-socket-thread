#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <stdbool.h>
typedef struct HashNode{
    struct HashNode* next;
    char *key;
    char *value;
}HashNode;

int Hashing (char* Key);

HashNode* HashNode_Create(char *Key, char *Value);
HashNode* HashNode_Insert(HashNode* Root, char* Key,char* Value);
HashNode* HashNode_Search(HashNode* Root,char *Key);
HashNode* HashNode_Delete(HashNode* Root,char *Key);


HashNode* HashTable[26]; 
int main(){
    for (int i = 0 ;i<26;++i)
    {
        HashTable[i] = (HashNode*)malloc(sizeof(HashNode));
        HashTable[i]->next = NULL;
    }
    int num;
    char key[30],value[30];
    while(1)
    {
        scanf("%d",&num);
        switch (num)
        {
            case 1:
                printf("----Insert----\n");
                scanf("%s %s",key,value);
                HashTable[Hashing(key)] = HashNode_Insert(HashTable[Hashing(key)],key,value);
                printf("--------------\n");
                break;
            case 2:
                printf("----Search----\n");
                scanf("%s",key);
                HashNode* node = HashNode_Search(HashTable[Hashing(key)],key);
                if(node != NULL)
                    printf("%s %s\n",node->key,node->value);
                else
                    printf("No find !\n");
                printf("--------------\n");
                break;
            case 3:
                printf("----Delete----\n");
                scanf("%s",key);
                HashTable[Hashing(key)] = HashNode_Delete(HashTable[Hashing(key)],key);
                printf("--------------\n");
            default:
                break;
        }
    }

}

int Hashing(char* Key)
{
    int hash = 0;
    for(int i = 0 ;i<strlen(Key);++i)
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
HashNode* HashNode_Search(HashNode* Root,char *Key){
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
        HashNode* temp = temp->next;
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