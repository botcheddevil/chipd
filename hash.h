#ifndef _HPCD_HASH_H_INCLUDED_
#define _HPCD_HASH_H_INCLUDED_

#include "http_parser.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#define HPCD_HASH_GUESS "guess"
#define HPCD_HASH_XOR "xor"
#define HPCD_HASH_BERNSTINE "bern"
#define HPCD_HASH_BERNSTINE31 "bern31"
#define HPCD_HASH_BERNSTINE37 "bern37"
#define HPCD_HASH_ROT "rot"
#define HPCD_HASH_MODBERNSTINE "modbern"
#define HPCD_HASH_SAX "sax"
#define HPCD_HASH_OAT "oat"
#define HPCD_HASH_MURMUR2 "murmur2"
#define HPCD_HASH_ELF "elf"
#define HPCD_HASH_JEN "jen"

/**
 * Define struct
 */

typedef struct hpcd_hash_item hpcd_hash_item;
typedef struct hpcd_hash_table hpcd_hash_table;

struct hpcd_hash_item
{
    hpcd_hash_item *next;
    char *key;
    char *content;
    size_t length;
    int depth;
};

/**
 * Define hash_table
 */

struct hpcd_hash_table
{
    unsigned int size;
    unsigned int item_count;
    unsigned int collision_count;
    unsigned int ( * algo ) ();
    hpcd_hash_item *table[];
};

/**
 * Hash functions
 */

unsigned int hpcd_hash_bernstine ( void *, int );
unsigned int hpcd_hash_modbernstine ( void *, int );
unsigned int hpcd_hash_bernstine31 ( void *, int );
unsigned int hpcd_hash_bernstine37 ( void *, int );
unsigned int hpcd_hash_xor ( void *, int );
unsigned int hpcd_hash_rot ( void *, int );
unsigned int hpcd_hash_sax ( void *, int );
unsigned int hpcd_hash_fnv ( void *, int );
unsigned int hpcd_hash_oat ( void *, int );
unsigned int hpcd_hash_elf ( void *, int );
unsigned int hpcd_hash_jen ( void *, int );
unsigned int hpcd_hash_murmur2 ( void *, int );

/**
 * Hash Operations
 */

hpcd_hash_item *hpcd_hash_item_fetch ( hpcd_hash_table *, char * );
int hpcd_hash_item_insert ( hpcd_hash_table *, hpcd_hash_item * );
int hpcd_hash_item_remove ( hpcd_hash_table *, char * );
void hpcd_hash_item_destroy ( hpcd_hash_item * );

/**
 * Hash table Operations
 */

hpcd_hash_table *hpcd_hash_table_create ( unsigned int ( * hash ) ( void *,
        int ), int );
void hpcd_hash_table_destroy ( hpcd_hash_table * );
int hpcd_hash_table_expand ( hpcd_hash_table * );

#endif
