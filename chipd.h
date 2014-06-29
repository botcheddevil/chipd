#define VERSION "0.1"
#define SERVER_NAME "chipd"
#define SERVER_NAME_FULL "HIgh Performance Content Delivery"

#define HTTP_VERSION "1.1"
#define HPCD_DEFAULT_PORT 80

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

static struct hpcd_cli_settings
{
    int verbose;
    char *hash_algorithm;
    char *directory[1024];
    char *port;
    char *filetypes;
    int help;
    int packet_cache;
    int gzip_content;
    int deflate_content;
} hpcd_cli_setting;
