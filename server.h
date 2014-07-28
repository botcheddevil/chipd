#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <strings.h>
#include <time.h>
#include "hash.h"
#include "load.h"

int hpcd_server_socket_fd;
typedef struct hpcd_server_http_request hpcd_server_http_request;

struct hpcd_server_http_request
{
	int *sock_fd;
	char *url;
	compression compression_type;
	int complete;
};

void *hpcd_server_handle_connection ( void * );
void hpcd_server_init ();

/**
 * Hash tables
 */

extern hpcd_hash_table *hpcd_hash_table_plain;
extern hpcd_hash_table *hpcd_hash_table_gzip;
extern hpcd_hash_table *hpcd_hash_table_deflate;
