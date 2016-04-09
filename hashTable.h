#ifndef _HASHTABLE_C
#define _HASHTABLE_C

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
    struct bucket buckets[10];	// We will use a fixed number of buckets: 10
}hashTable_t;

#endif