
/**
 * Define struct
 */

typedef struct item item;
typedef struct hashtable hashtable;

struct item
{
    item *next;
    char *key;
    char *content;
    size_t length;
    int depth;
};

/**
 * Define hash_table
 */

struct hashtable
{
    unsigned int size;
    unsigned int item_count;
    unsigned int collision_count;
    unsigned int ( * algo ) ();
    item *table[];
};

/**
 * Hash functions
 */

unsigned int hash_bernstine ( void *, int );
unsigned int hash_modbernstine ( void *, int );
unsigned int hash_bernstine31 ( void *, int );
unsigned int hash_bernstine37 ( void *, int );
unsigned int hash_xor ( void *, int );
unsigned int hash_rot ( void *, int );
unsigned int hash_sax ( void *, int );
unsigned int hash_fnv ( void *, int );
unsigned int hash_oat ( void *, int );
unsigned int hash_elf ( void *, int );
unsigned int hash_jen ( void *, int );
unsigned int hash_murmur2 ( void *, int );

/**
 * Hash Operations
 */

item *hash_fetch ( hashtable *, char * );
int hash_insert ( hashtable *, item * );
int hash_remove ( hashtable *, char * );

/**
 * Hash table Operations
 */

hashtable *hash_createtable ( unsigned int ( * hash ) ( void *, int ), int );
void hash_destroytable ( hashtable * );
int hash_expandtable ( hashtable * );