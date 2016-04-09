#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_
#include <stdint.h>

typedef struct bucketNode{
    char* data;
    struct bucketNode* next;    
}bucketNode_t;

typedef struct bucket{
    bucketNode_t *head;
    bucketNode_t *tail;
	int size;
}bucket_t;

typedef struct hashTable{
    bucket_t buckets[10];	// We will use a fixed number of buckets: 10
}hashTable_t;

// Function prototypes:
void HashTable_init(hashTable_t*);
int lookupHashTable(char*, hashTable_t*);
int calculateHash(char* );
uint16_t fletcher16( uint8_t const *data, size_t bytes );

#endif
