#include <stdio.h>
#include <string.h>
#include "hash.h"


void hash_expand ()
{

}



unsigned xor_hash ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++)
        h ^= p[i];

    return h;

}

item *
hash_fetch(item *ht[255], char *key)
{

    unsigned int index = xor_hash ( key , strlen(key) );

    if (ht[index] == 0)
        return NULL;

    item *itm = ht[index];

    while (1)
    {
        if (strcmp(itm->key, key) == 0)
            return itm;

        if (itm->next == 0)
            return NULL;
        else
            itm = itm->next;
    }

}

/**
 * Insert Item to hash table
 *
 * @param ht  item** hash table
 * @param itm item*  item to insert
 */

void hash_insert(item *ht[255], item *itm)
{

    unsigned int index = xor_hash ( itm->key , strlen(itm->key) );

    /**
     * Check if location is occupied
     */

    if (ht[index] == 0)
    {
        ht[index] = itm;
    }
    else
    {
        item *nextitem = ht[index];

        while (nextitem->next != 0)
            nextitem = nextitem->next;

        nextitem->next = itm;
    }

}