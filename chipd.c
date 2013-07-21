
/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/*
 *  chipd - in memory content delivery webserver
 *
 *  Copyright 2013 vhv.  All rights reserved.
 *
 *  Use and distribution licensed under the BSD license.  See
 *  the LICENSE file for full text.
 *
 *  Authors:
 *      Anshuk Kumar <anshukk@gmail.com>
 */

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

int sockfd;

void *handle_connection ( void *arg );
item *hash_table[255] = {0};

item *
load_notfound_memory ( char *status )
{

    /**
     * --------------------
     * Load files to memory
     * --------------------
     */

    unsigned long RESPONSE_LENGTH;
    const char *separator = "/";
    item *itm = ( item * ) malloc ( sizeof ( item ) );

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

    /** Populate item struct **/
    itm->content = response;

    itm->key = status;

    itm->length = ( size_t ) RESPONSE_LENGTH;
    itm->next = 0;

    return itm;
}

item *
load_file_memory ( char *filename, size_t base_url_length )
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
    item *itm = ( item * ) malloc ( sizeof ( item ) );

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

    /** Populate item struct **/
    itm->content = response;

    itm->key = ( char * ) malloc ( ( strlen ( filename ) - base_url_length + 1 ) *
                                   sizeof ( char ) );
    strcpy ( itm->key, filename + ( base_url_length * sizeof ( char ) ) );

    itm->length = ( size_t ) RESPONSE_LENGTH;
    itm->next = 0;

    return itm;
}

void
load_dir ( const char *dir_name, item *ht[255], size_t base_url_length )
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
                load_dir ( path, ht, base_url_length );
            }

        }
        else
        {

            hash_insert ( ht, load_file_memory ( filename, base_url_length ) );
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

void error ( const char *msg )
{
    perror ( msg );
    exit ( 1 );
}

void sig_handler ( int signo )
{
    if ( signo == SIGINT )
    {
        printf ( "received SIGINT\n" );
        close ( sockfd );
    }
}

int main ( int argc, char *argv[] )
{
    int *newsockfd, portno, n,
        ctr = 0 ;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t thread1;
    pthread_attr_t attr;

    /* Handle SIGINT */
    struct sigaction a;
    a.sa_handler = sig_handler;
    a.sa_flags = 0;
    sigemptyset ( &a.sa_mask );
    sigaction ( SIGINT, &a, NULL );

    /* Create detached thread attribute */
    pthread_attr_init ( &attr );
    pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );

    /**
     * If number of arguments are less than two then
     * then raise an error
     */
    if ( argc < 2 )
    {
        fprintf ( stderr,"ERROR, no port provided\n" );
        exit ( 1 );
    }

    /**
     * --------------------
     * Load files in a directory
     * ____________________
     */
    char *dir_name = "/Users/anshukkumar/chipd/tests/dummy";
    load_dir ( dir_name, hash_table, strlen ( dir_name ) );
    hash_insert ( hash_table, load_notfound_memory ( "404" ) );

    /* Lets create a socket */
    printf ( "Creating socket\n" );
    sockfd = socket ( AF_INET, SOCK_STREAM, 0 );

    /**
     * If we are unable to create socket
     * then raise an error
     */
    if ( sockfd < 0 )
    {
        error ( "ERROR opening socket" );
    }

    /* clear all garbage from serv_addr */
    bzero ( ( char * ) &serv_addr, sizeof ( serv_addr ) );

    /* get the port number from argument and convert it to string */
    portno = atoi ( argv[1] );

    /* configure setting for serv_addr */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons ( portno );

    /* configure the socket server addr */
    printf ( "Binding socket\n" );
    if ( bind ( sockfd, ( struct sockaddr * ) &serv_addr,
                sizeof ( serv_addr ) ) < 0 )
    {
        error ( "ERROR on binding" );
    }

    /* start listening on socket */
    printf ( "Starting to Listen %d\n", HTTP_REQUEST );
    listen ( sockfd,5 );

    clilen = sizeof ( cli_addr );

    /* Main Server Loop */
    while ( 1 )
    {
        /* Start Accepting connections */
        printf ( "Accepting new connection\n" );
        newsockfd = ( int * ) malloc ( sizeof ( int ) );
        *newsockfd = accept ( sockfd,
                              ( struct sockaddr * ) &cli_addr,
                              &clilen );

        /* Create independent threads each of which will execute function */
        pthread_create ( &thread1, &attr, handle_connection, ( void * ) newsockfd );

        /* Found a new connection */
        printf ( "Accepted!! %d\n", ++ctr );
        if ( *newsockfd < 0 )
        {
            error ( "ERROR on accept" );
        }
    }

    return 0;
}

int on_url ( http_parser *parser, const char *at, size_t length )
{
    int n,
        newsockfd = * ( ( int * ) parser->data );
    item *itm;
    char *key = ( char * ) malloc ( ( int ) length * sizeof ( char ) );
    strncpy ( key, at, length );

    printf ( "Url: %.*s\n", ( int ) length, key );

    itm = hash_fetch ( hash_table, key );


    if ( itm == NULL )
    {
        printf ( "Not Found\n" );
        itm = hash_fetch ( hash_table, "404" );
    }

    n = write ( newsockfd, itm->content, itm->length );

    if ( n < 0 )
    {
        error ( "ERROR writing to socket" );
    }



    return 0;
}

void *handle_connection ( void *arg )
{
    char buffer[80 * 1024];
    int n,
        newsockfd = * ( ( int * ) arg );
    free ( arg );

    http_parser_settings settings;

    memset ( &settings, 0, sizeof ( settings ) );
    settings.on_url = on_url;


    /* Clear the buffer */
    bzero ( buffer, 80 * 1024 );

    http_parser *parser = malloc ( sizeof ( http_parser ) );
    http_parser_init ( parser, HTTP_REQUEST );
    parser->data = &newsockfd;

    /* Reading from buffer */
    printf ( "Reading from buffer\n" );
    n = recv ( newsockfd, buffer, 80 * 1024, 0 );

    if ( n < 0 )
    {
        error ( "ERROR reading from socket" );
    }

    size_t nparsed = http_parser_execute ( parser, &settings, buffer, n );

    if ( nparsed != ( size_t ) n )
    {
        fprintf ( stderr,
                  "Error: %s (%s)\n",
                  http_errno_description ( HTTP_PARSER_ERRNO ( parser ) ),
                  http_errno_name ( HTTP_PARSER_ERRNO ( parser ) ) );
    }

    close ( newsockfd );

    return NULL;
}
