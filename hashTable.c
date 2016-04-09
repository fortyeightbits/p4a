#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#include "hashTable.h"

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

void HashTable_init(hashTable_t* hashtable)
{
    int size = 10;
    int i;
    for(i = 0; i < size; ++i)
    {
        bucketNode_t* temp = (bucketNode_t*)malloc(sizeof(bucketNode_t));
        temp->data = (char*)malloc(100*sizeof(char));
        temp->next = NULL;

        hashtable->buckets[i].head = temp;
        hashtable->buckets[i].tail = temp;
    }
}

// Main hash table lookup function. Appends to table and returns 0 if not already in table. If in table returns 1;
int lookupHashTable(char* data, hashTable_t* hashtable)
{
    int bucketIndex = calculateHash(data);
    int matched = -1;
    //hashtable->buckets[bucketIndex]
    pthread_mutex_lock(&hashlock);
    bucketNode_t* parser = hashtable->buckets[bucketIndex].head;
    // Search for data in nodes of bucket (if any)
    while(parser != NULL)
    {
        matched = strcmp(data, parser->data);
        if (matched == 0)
            break;
        parser = parser->next;
    }

    if (matched == 0)
    {
        // if found, then exit with 1.
        return 1;
    }
    else if (matched != 0)
    {
        // if not found, then append to tail of that bucket and return 0.
        if(hashtable->buckets[bucketIndex].head->data == NULL){
            strcpy(hashtable->buckets[bucketIndex]->tail->data, data);
        }
        else{
            bucketNode_t* temp = (bucketNode_t*)malloc(sizeof(bucketNode_t));
            temp->data = (char*)malloc(100*sizeof(char));
            strcpy(temp->data, data);
            temp->next = NULL;
            hashtable->buckets[bucketIndex].tail->next = temp;
            hashtable->buckets[bucketIndex].tail = temp;
        }
        pthread_mutex_unlock(&hashlock);
        return 0;
    }
}

int calculateHash(char* data)
{
    int finalHash;
    // get the length of the string in chars(bytes)
    int lengthInBytes = strlen(data);
    char* copiedData = strdup(data);
    finalHash = (fletcher16(copiedData, lengthInBytes)%10);
    free(copiedData);
    return finalHash;
}

uint16_t fletcher16( uint8_t const *data, size_t bytes )
{
        uint16_t sum1 = 0xff, sum2 = 0xff;
        size_t tlen;

        while (bytes) {
                tlen = bytes >= 20 ? 20 : bytes;
                bytes -= tlen;
                do {
                        sum2 += sum1 += *data++;
                } while (--tlen);
                sum1 = (sum1 & 0xff) + (sum1 >> 8);
                sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }
        /* Second reduction step to reduce sums to 8 bits */
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
        return sum2 << 8 | sum1;
}

