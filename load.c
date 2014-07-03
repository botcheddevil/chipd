#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "load.h"
#include "chipd.h"
#include "zlib.h"

void *hpcd_load_httpcontent (
    headers *httphds, void *content,
    size_t content_length )
{

    char *response;
    int response_length;

    const char *key_status = "HTTP/" HTTP_VERSION " ";
    const char *key_content_type = "Content-Type: ";
    const char *key_etag = "Etag: ";
    const char *key_content_encoding = "Content-Encoding: ";
    const char *key_content_length = "Content-Length: ";
    const char *key_cache_control = "Cache-Control: ";
    const char *key_connection = "Connection: ";
    const char *key_server = "Server: ";

    /* status */
    char *status = ( char * )
                   calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_status ) + ( int ) strlen (
                                httphds->status ) ) );

    strcat ( status, key_status );
    strcat ( status, httphds->status );

    /* content type */
    char *content_type = ( char * )
                         calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_content_type ) +
                                  ( int ) strlen ( httphds->content_type ) ) );

    strcat ( content_type, key_content_type );
    strcat ( content_type, httphds->content_type );

    /* etag */
    char *etag = ( char * )
                 calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_etag ) + ( int ) strlen (
                              httphds->etag ) ) );

    strcat ( etag, key_etag );
    strcat ( etag, httphds->etag );

    /* server */
    char *server = ( char * )
                   calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_server ) + ( int ) strlen (
                                SERVER_NAME "/" VERSION ) ) );

    strcat ( server, key_server );
    strcat ( server, SERVER_NAME "/" VERSION );

    /*content encoding*/
    char *content_encoding = ( char * )
                             calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_content_encoding ) +
                                      ( int ) strlen ( httphds->content_encoding ) ) );

    strcat ( content_encoding, key_content_encoding );
    strcat ( content_encoding, httphds->content_encoding );

    char *cache_control = ( char * )
                          calloc ( 1, sizeof ( char ) * ( ( int ) strlen ( key_cache_control ) +
                                   ( int ) strlen ( "no-cache" ) ) );

    strcat ( cache_control, key_cache_control );
    strcat ( cache_control, "no-cache" );

    printf ( "%s\n", status );
    printf ( "%s\n", content_type );
    printf ( "%s\n", etag );
    printf ( "%s\n", server );
    printf ( "%s\n", content_encoding );
    printf ( "%s\n", cache_control );

    int err;
    Byte *compr, *uncompr;
    uLong comprLen = ( int ) content_length *
                     sizeof ( int ); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;

    compr    = ( Byte * ) calloc ( ( uInt ) comprLen, 1 );
    uncompr  = ( Byte * ) calloc ( ( uInt ) uncomprLen, 1 );

    printf ( "before strlen\n" );

    err = compress ( compr, &comprLen, ( const Bytef * ) content,
                     ( uLong ) content_length + 1 );
    if ( err != Z_OK )
    {
        fprintf ( stderr, "%s error: %d\n", "compress", err );
        exit ( 1 );
    }

    strcpy ( ( char * ) uncompr, "garbage" );

    err = uncompress ( uncompr, &uncomprLen, compr, comprLen );

    if ( err != Z_OK )
    {
        fprintf ( stderr, "%s error: %d\n", "uncompress", err );
        exit ( 1 );
    }

    printf ( "Length Before compression %d\n",
             ( int ) strlen ( ( char * ) content ) );

    printf ( "Length After compression %lu\n", comprLen );

    printf ( "%s\n", uncompr );

    return ( void * ) content;

}

void
hpcd_load_directory ( const char *dir_name, hpcd_hash_table *ht,
                      size_t base_url_length )
{
    DIR *d;
    char *filename;

    /* Open the directory specified by "dir_name". */
    d = opendir ( dir_name );

    /* Check it was opened. */
    if ( ! d )
    {
        fprintf ( stderr, "Cannot open directory '%s': %s\n",
                  dir_name, strerror ( errno ) );
        exit ( EXIT_FAILURE );
    }

    /** Start looking for files to load**/
    while ( 1 )
    {
        struct dirent *entry;
        const char *d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir ( d );
        if ( ! entry )
        {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }

        d_name = entry->d_name;

        /* Print the name of the file and directory. */
        int length = strlen ( dir_name ) + strlen ( d_name ) + 1;
        filename = ( char * ) calloc ( length, sizeof ( char ) );

        strcat ( filename, dir_name );
        strcat ( filename, "/" );
        strcat ( filename, d_name );

        /* See if "entry" is a subdirectory of "d". */
        if ( entry->d_type & DT_DIR )
        {

            /* Check that the directory is not "d" or d's parent. */

            if ( strcmp ( d_name, ".." ) != 0 &&
                    strcmp ( d_name, "." ) != 0 )
            {
                int path_length;
                char path[PATH_MAX];

                path_length = snprintf ( path, PATH_MAX,
                                         "%s/%s", dir_name, d_name );

                if ( path_length >= PATH_MAX )
                {
                    fprintf ( stderr, "Path length has got too long.\n" );
                    exit ( EXIT_FAILURE );
                }
                /* Recursively call "list_dir" with the new path. */
                printf ( "%s", filename );
                printf ( " : Entering" );
                printf ( "\n" );
                hpcd_load_directory ( path, ht, base_url_length );
            }

        }
        else
        {

            hpcd_hash_item_insert ( ht, hpcd_load_file ( filename, base_url_length ) );
            printf ( "%s", filename );
            printf ( " : Loaded\n" );

        }
    }
    /* After going through all the entries, close the directory. */
    if ( closedir ( d ) )
    {
        fprintf ( stderr, "Could not close '%s': %s\n",
                  dir_name, strerror ( errno ) );
        exit ( EXIT_FAILURE );
    }
}

hpcd_hash_item *
hpcd_load_notfound ( )
{

    /**
     * --------------------
     * Load files to memory
     * --------------------
     */

    unsigned long RESPONSE_LENGTH;
    const char *separator = "/";
    hpcd_hash_item *itm = ( hpcd_hash_item * ) malloc ( sizeof ( hpcd_hash_item ) );

    /** Get File Content */
    char *content = "<h2>404 File Not Found</h2> chipd/0.1";

    /** Build response **/
    char *response_header_part1 =
        "HTTP/1.1 404 Not Found\n"
        "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
        "Server: chipd/0.1\n"
        "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
        "ETag: \"56d-9989200-1132c580\"\n"
        "Content-Type: text/html\n"
        "Content-Length: ";

    char *response_header_part3 =
        "\n"
        "Accept-Ranges: bytes\n"
        "Connection: close\n"
        "\n";

    RESPONSE_LENGTH = strlen ( response_header_part1 ) + 10 + strlen (
                          response_header_part3 ) + strlen ( content );
    char *response = ( char * ) malloc ( RESPONSE_LENGTH );

    /** Concat all string into response **/
    snprintf ( response, ( size_t ) RESPONSE_LENGTH, "%s%lu%s",
               response_header_part1, ( unsigned long ) strlen ( content ),
               response_header_part3 );

    /** Add file content **/
    memcpy ( response + ( int ) strlen ( response ), content, strlen ( content ) );

    /** Populate hpcd_hash_item struct **/
    itm->content = response;

    itm->key = "404";

    itm->length = ( size_t ) RESPONSE_LENGTH;
    itm->next = 0;

    return itm;
}


hpcd_hash_item *
hpcd_load_file ( char *filename, size_t base_url_length )
{

    /**
     * --------------------
     * Load files to memory
     * --------------------
     */

    FILE *fp;

    /** Get File Length */
    struct stat info;
    unsigned long RESPONSE_LENGTH;
    const char *separator = "/";
    hpcd_hash_item *itm = ( hpcd_hash_item * ) malloc ( sizeof ( hpcd_hash_item ) );

    stat ( filename, &info );

    /** Get File Content */
    char *content = ( char * ) malloc ( info.st_size * sizeof ( char ) );
    fp = fopen ( filename, "rb" );
    fread ( content, info.st_size, 1, fp );
    fclose ( fp );

    /** Build response **/
    char *response_header_part1 =
        "HTTP/1.1 200 OK\n"
        "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
        "Server: chipd/0.1\n"
        "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
        "ETag: \"56d-9989200-1132c580\"\n"
        "Content-Type: image/gif\n"
        "Content-Length: ";

    char *response_header_part3 =
        "\n"
        "Accept-Ranges: bytes\n"
        "Connection: close\n"
        "\n";

    RESPONSE_LENGTH = strlen ( response_header_part1 ) + 10 + strlen (
                          response_header_part3 ) + ( unsigned long ) info.st_size;
    char *response = ( char * ) malloc ( RESPONSE_LENGTH );

    /** Concat all string into response **/
    snprintf ( response, ( size_t ) RESPONSE_LENGTH, "%s%lu%s",
               response_header_part1, ( unsigned long ) info.st_size, response_header_part3 );

    /** Add file content **/
    memcpy ( response + ( int ) strlen ( response ), content, info.st_size );

    /** Populate hpcd_hash_item struct **/
    itm->content = response;

    itm->key = ( char * ) malloc ( ( strlen ( filename ) - base_url_length + 1 ) *
                                   sizeof ( char ) );
    strcpy ( itm->key, filename + ( base_url_length * sizeof ( char ) ) );

    itm->length = ( size_t ) RESPONSE_LENGTH;
    itm->next = 0;

    return itm;
}
