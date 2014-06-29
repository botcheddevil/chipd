#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "load.h"
#include "chipd.h"
#include "zlib.h"

void *hpcd_load_httpcontent_memory (
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
