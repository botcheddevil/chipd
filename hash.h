
#include "http_parser.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

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

/**
 * Hash table Operations
 */

hpcd_hash_table *hpcd_hash_table_create ( unsigned int ( * hash ) ( void *, int ), int );
void hpcd_hash_table_destroy ( hpcd_hash_table * );
int hpcd_hash_table_expand ( hpcd_hash_table * );

/**
 * Hash tables
 */

hpcd_hash_table *hpcd_hash_table_plain;
hpcd_hash_table *hpcd_hash_table_gzip;
hpcd_hash_table *hpcd_hash_table_deflate;

hpcd_hash_table *hpcd_hash_table_bernstine;
hpcd_hash_table *hpcd_hash_table_modbernstine;
hpcd_hash_table *hpcd_hash_table_bernstine31;
hpcd_hash_table *hpcd_hash_table_bernstine37;
hpcd_hash_table *hpcd_hash_table_xor;
hpcd_hash_table *hpcd_hash_table_rot;
hpcd_hash_table *hpcd_hash_table_sax;
hpcd_hash_table *hpcd_hash_table_fnv;
hpcd_hash_table *hpcd_hash_table_oat;
hpcd_hash_table *hpcd_hash_table_elf;
hpcd_hash_table *hpcd_hash_table_jen;
hpcd_hash_table *hpcd_hash_table_murmur2;
