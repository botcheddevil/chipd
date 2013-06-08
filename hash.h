typedef struct item item;

struct item {
   item *next;
   char *key;
   char *content;
   size_t length;
};

unsigned xor_hash ( void *, int);
int hash_fetch(item **, char *);
void hash_insert(item **, char *, int);