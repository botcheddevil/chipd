#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hash.h"

int main()
{

    hashtable *ht;
    int ctr;
    char *pos, *key, *value, *nextitemkey;
    item *itm;

    printf ( "------------------\n" );
    printf ( "Testing Hash table\n" );
    printf ( "------------------\n" );

    ht = hash_createtable ( hash_sax, 324358 );

    if ( !ht )
    {
        printf ( "Hash table could not be created\n" );
        exit ( 0 );
    }

    printf ( "Hash table created\n" );
    printf ( "Hash table size: %d\n", ht->size );

    /**
     * Open Dictionary
     */

    printf ( "Inserting items to hashtable...\n" );

    FILE *file = fopen ( "./tests/de-en.txt", "r" );
    if ( file != NULL )
    {

        char line [ 256 ]; /* or other suitable maximum line size */
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
        {
            if ( line[0] != '#' )
            {
                pos = strstr ( line, "::" );
                if ( pos != NULL )
                {

                    *pos = '\0';
                    key = malloc ( sizeof ( char ) * ( ( int ) strlen ( line )+1 ) );
                    value = malloc ( sizeof ( char ) * ( ( int ) strlen ( pos+3 )+1 ) );
                    strcpy ( key, line );
                    strcpy ( value, ( pos+3 ) );

                    itm = ( item * ) calloc ( 1, sizeof ( item ) );
                    itm->key = key;
                    itm->content = value;

                    hash_insert ( ht, itm );

                }
            }
        }

        fclose ( file );
    }
    else
    {
        perror ( "./tests/de-en.txt" ); /* why didn't the file open? */
    }

    key = ( char * ) malloc ( ( strlen ( "Abblendlicht {n} " ) + 1 ) * sizeof (
                                  char ) );
    strcpy ( key, "Abblendlicht {n} " );

    // Fetch an Item
    itm = hash_fetch ( ht, key );
    if ( itm != NULL )
    {
        printf ( "Item fetched for key: %s\n", key );
        printf ( "%s\n", itm->content );

        if ( itm->next != 0 )
        {
            printf ( "Next Item for key: %s\n", key );
            printf ( "%s\n", itm->next->key );
            printf ( "%s\n", itm->next->content );
            nextitemkey = itm->next->key;
        }
    }
    else
    {
        printf ( "Not Found Key: %s\n", key );
    }


    if ( hash_remove ( ht, key ) )
    {
        printf ( "Deleted key: %s\n", key );
    }
    else
    {
        printf ( "Unable to Delete key: %s\n", key );
    }


    // Fetch an Item
    itm = hash_fetch ( ht, key );
    if ( itm != NULL )
    {
        printf ( "Item fetched for key: %s\n", key );
        printf ( "%s\n", itm->content );
    }
    else
    {
        printf ( "Not Found Key: %s\n", key );
    }

    // Fetch an Item
    itm = hash_fetch ( ht, nextitemkey );
    if ( itm != NULL )
    {
        printf ( "Item fetched for key: %s\n", nextitemkey );
        printf ( "%s\n", itm->content );
    }
    else
    {
        printf ( "Not Found Key: %s\n", nextitemkey );
    }

    printf ( "Hash table collsions: %d\n", ht->collision_count );
    printf ( "Hash table item count: %d\n", ht->item_count );

    printf ( "---------------\n" );
    printf ( "Collision Stats\n" );
    printf ( "---------------\n" );

    int stats[256] = {0};

    for ( ctr=0; ctr < ht->size; ctr++ )
    {
        if ( ht->table[ctr] != 0 )
        {
            if ( ht->table[ctr]->depth < 256 )
            {
                stats[ht->table[ctr]->depth]++;
            }
            else
            {
                stats[255]++;
            }
        }
    }

    printf ( "-------------\n" );
    printf ( "Depth   Count\n" );
    printf ( "-------------\n" );

    for ( ctr=0; ctr<256; ctr++ )
    {
        if ( stats[ctr] != 0 )
        {
            printf ( "%d       %d\n",ctr, stats[ctr] );
        }
    }

    hash_destroytable ( ht );
    printf ( "Hash table distroyed\n" );

    return 0;
}