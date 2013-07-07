typedef struct item item;

struct item
{
    item *next;
    char *key;
    char *content;
    size_t length;
};

struct hash_table
{
    unsigned int length;
    unsigned int item_count;
    item *table[];
};

unsigned xor_hash ( void *, int);
item *hash_fetch(item *[], char *);
void hash_insert(item *[], item *);