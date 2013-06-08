#include <stdio.h>
#include "hash.h"

unsigned xor_hash ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++)
        h ^= p[i];

    return h;

}

int hash_fetch(item **ht, char *key)
{

    unsigned int index = xor_hash ( key , sizeof(key) );

    return ht[index]->length;

}

void hash_insert(item **ht, char *key, int val)
{

    unsigned int index = xor_hash ( key , sizeof(key) );

    item i;
    /*
    i.val = val;
    i.key = key;
    */
    ht[index] = &i;
}