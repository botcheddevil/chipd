#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <strings.h>
#include "hash.h"

int hpcd_server_socket_fd;

void *hpcd_server_handle_connection ( void * );
void hpcd_server_init ();

/**
 * Hash tables
 */

extern hpcd_hash_table *hpcd_hash_table_plain;
extern hpcd_hash_table *hpcd_hash_table_gzip;
extern hpcd_hash_table *hpcd_hash_table_deflate;
