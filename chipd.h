#include <stdio.h>
#include <getopt.h>
#include "signals.h"
#include "load.h"

#define VERSION "0.1"
#define SERVER_NAME "chipd"
#define SERVER_NAME_FULL "HIgh Performance Content Delivery"

#define HTTP_VERSION "1.1"
#define HPCD_DEFAULT_PORT 80

typedef struct hpcd_cli_settings hpcd_cli_settings;

struct hpcd_cli_settings
{
    int verbose;
    char *hash_algorithm;
    char directory[1024];
    char *port;
    char *filetypes;
    int help;
    int packet_cache;
    int gzip_content;
    int deflate_content;
};
