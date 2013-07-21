#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

/**
 * Hash functions
 */

unsigned hash_xor ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h ^= p[i];
    }

    return h;
}

unsigned hash_bernstine ( void *key, int len )
{
    unsigned int h;
    char *p = ( char * ) key;
    int i;

    h = 0;
    for ( i = 0; i < len; i++ )
    {
        h = 33 * h + p[i];
    }

    return h;
}

unsigned hash_bernstine31 ( void *key, int len )
{
    unsigned int h;
    char *p = ( char * ) key;
    int i;

    h = 0;
    for ( i = 0; i < len; i++ )
    {
        h = 31 * h + p[i];
    }

    return h;
}

unsigned hash_bernstine37 ( void *key, int len )
{
    unsigned int h;
    char *p = ( char * ) key;
    int i;

    h = 0;
    for ( i = 0; i < len; i++ )
    {
        h = 31 * h + p[i];
    }

    return h;
}

unsigned hash_rot ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h = ( h << 4 ) ^ ( h >> 28 ) ^ p[i];
    }

    return h;
}

unsigned hash_modbernstine ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h = 33 * h ^ p[i];
    }

    return h;
}

unsigned hash_sax ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h ^= ( h << 5 ) + ( h >> 2 ) + p[i];
    }

    return h;
}

unsigned hash_fnv ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 2166136261;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h = ( h * 16777619 ) ^ p[i];
    }

    return h;
}

unsigned hash_oat ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h += p[i];
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}

unsigned int hash_murmur2 ( void *key, int len )
{
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    unsigned int seed = 5381;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    unsigned int h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = ( const unsigned char * ) key;

    while ( len >= 4 )
    {
        unsigned int k = * ( unsigned int * ) data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch ( len )
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return ( unsigned int ) h;
}

unsigned hash_elf ( void *key, int len )
{
    unsigned char *p = key;
    unsigned h = 0, g;
    int i;

    for ( i = 0; i < len; i++ )
    {
        h = ( h << 4 ) + p[i];
        g = h & 0xf0000000L;

        if ( g != 0 )
        {
            h ^= g >> 24;
        }

        h &= ~g;
    }

    return h;
}

#define hashsize(n) ( 1U << (n) )
#define hashmask(n) ( hashsize ( n ) - 1 )

#define mix(a,b,c) \
{ \
    a -= b; a -= c; a ^= ( c >> 13 ); \
    b -= c; b -= a; b ^= ( a << 8 ); \
    c -= a; c -= b; c ^= ( b >> 13 ); \
    a -= b; a -= c; a ^= ( c >> 12 ); \
    b -= c; b -= a; b ^= ( a << 16 ); \
    c -= a; c -= b; c ^= ( b >> 5 ); \
    a -= b; a -= c; a ^= ( c >> 3 ); \
    b -= c; b -= a; b ^= ( a << 10 ); \
    c -= a; c -= b; c ^= ( b >> 15 ); \
}

unsigned int hash_jen ( void *key, int length )
{
    unsigned a, b;
    unsigned int c = 2556445788;
    unsigned char *k = ( unsigned char * ) key;
    unsigned len = length;

    a = b = 0x9e3779b9;

    while ( len >= 12 )
    {
        a += ( k[0] + ( ( unsigned ) k[1] << 8 )
               + ( ( unsigned ) k[2] << 16 )
               + ( ( unsigned ) k[3] << 24 ) );
        b += ( k[4] + ( ( unsigned ) k[5] << 8 )
               + ( ( unsigned ) k[6] << 16 )
               + ( ( unsigned ) k[7] << 24 ) );
        c += ( k[8] + ( ( unsigned ) k[9] << 8 )
               + ( ( unsigned ) k[10] << 16 )
               + ( ( unsigned ) k[11] << 24 ) );

        mix ( a, b, c );

        k += 12;
        len -= 12;
    }

    c += length;

    switch ( len )
    {
    case 11:
        c += ( ( unsigned ) k[10] << 24 );
    case 10:
        c += ( ( unsigned ) k[9] << 16 );
    case 9 :
        c += ( ( unsigned ) k[8] << 8 );
        /* First byte of c reserved for length */
    case 8 :
        b += ( ( unsigned ) k[7] << 24 );
    case 7 :
        b += ( ( unsigned ) k[6] << 16 );
    case 6 :
        b += ( ( unsigned ) k[5] << 8 );
    case 5 :
        b += k[4];
    case 4 :
        a += ( ( unsigned ) k[3] << 24 );
    case 3 :
        a += ( ( unsigned ) k[2] << 16 );
    case 2 :
        a += ( ( unsigned ) k[1] << 8 );
    case 1 :
        a += k[0];
    }

    mix ( a, b, c );

    return c;
}


/**
 * Hash table Operations
 */

hashtable *hash_createtable ( unsigned int ( * hash ) ( void *, int ),
                              int size )
{
    hashtable *ht;

    /* Space is allocated for the struct */
    ht = ( hashtable * )
         calloc ( 1, sizeof ( hashtable ) + sizeof ( item * ) * size );

    ht->algo = hash;
    ht->size = size;

    return ht;
}

void hash_destroyitem ( item *itm )
{

    if ( itm == 0 )
    {
        return;
    }
    hash_destroyitem ( itm->next );
    free ( itm->key );
    free ( itm->content );

}

void hash_destroytable ( hashtable *ht )
{
    int ctr = 0;
    item *nextitem;

    for ( ctr = 0; ctr < ht->size; ctr++ )
    {
        hash_destroyitem ( ht->table[ctr] );
    }

    free ( ht );
}

hashtable *hash_resizetable ( hashtable *ht, int size )
{
    int ctr = 0;
    item *nextitem;
    item *temp_nextitem;
    hashtable *newht = ( hashtable * )
                       calloc ( 1, sizeof ( hashtable ) + sizeof ( item * ) * size );

    newht->size = size;
    newht->algo = ht->algo;

    for ( ctr = 0; ctr < ht->size; ctr++ )
    {
        nextitem = ht->table[ctr];
        do
        {
            hash_insert ( newht, nextitem );
            if ( nextitem->next == 0 )
            {
                break;
            }
            temp_nextitem = nextitem;
            nextitem = nextitem->next;
            temp_nextitem->next = 0;
        }
        while ( 1 );
    }

    free ( ht );
    return newht;
}

/**
 * Insert Item to hash table
 *
 * @param ht  item** hash table
 * @param itm item*  item to insert
 *
 * @return NULL if item is not found
 *         item* if item is found
 */

item *hash_fetch ( hashtable *ht, char *key )
{

    unsigned int index = ( *ht->algo ) ( key , strlen ( key ) );

    index = index % ht->size;

    if ( ht->table[index] == 0 )
    {
        return NULL;
    }

    item *itm = ht->table[index];

    while ( 1 )
    {
        if ( strcmp ( itm->key, key ) == 0 )
        {
            return itm;
        }

        if ( itm->next == 0 )
        {
            return NULL;
        }
        else
        {
            itm = itm->next;
        }
    }

}

/**
 * Insert Item to hash table
 *
 * @param ht  item** hash table
 * @param itm item*  item to insert
 */

int hash_insert ( hashtable *ht, item *itm )
{

    unsigned int index = ( *ht->algo ) ( itm->key , strlen ( itm->key ) );

    index = index % ht->size;

    if ( ht->table[index] == 0 )
    {
        ht->table[index] = itm;
    }
    else
    {
        item *nextitem = ht->table[index];

        do
        {

            if ( strcmp ( nextitem->key, itm->key ) == 0 )
            {
                free ( nextitem->content );
                free ( itm->key );

                nextitem->content = itm->content;
                free ( itm );
                ht->item_count++;
                return 1;
            }

            if ( nextitem->next == 0 )
            {
                break;
            }
            else
            {
                nextitem = nextitem->next;
            }

        }
        while ( 1 );

        ht->collision_count++;
        ht->table[index]->depth++;
        nextitem->next = itm;
    }

    ht->item_count++;

    return 1;

}

/**
 * Remove Item to hash table
 *
 * @param ht  item** hash table
 * @param itm item*  item to insert
 */

int hash_remove ( hashtable *ht, char *key )
{

    unsigned int index = ( *ht->algo ) ( key , strlen ( key ) );
    item **previousitem;
    int collided = 0;

    index = index % ht->size;

    if ( ht->table[index] != 0 )
    {

        item *nextitem = ht->table[index];
        previousitem = &ht->table[index];

        while ( nextitem != 0 )
        {
            if ( strcmp ( nextitem->key, key ) == 0 )
            {

                ( *previousitem ) = nextitem->next;
                free ( nextitem );
                ht->item_count--;

                if ( collided )
                {
                    ht->collision_count--;
                    ht->table[index]->depth--;
                }

                return 1;

            }
            else
            {

                collided = 1;
                previousitem = &nextitem->next;
                nextitem = nextitem->next;

            }
        }

    }

    return 0;

}