#include "server.h"

extern struct hpcd_cli_settings
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
} hpcd_cli_setting;

void hpcd_server_init() {

    int *newsockfd, ctr = 0;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t thread1;
    pthread_attr_t attr;


    /* Create detached thread attribute */
    pthread_attr_init ( &attr );
    pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );

    /* Lets create a socket */
    printf ( "Creating socket\n" );
    hpcd_server_socket_fd = socket ( AF_INET, SOCK_STREAM, 0 );

    /**
     * If we are unable to create socket
     * then raise an error
     */
    if ( hpcd_server_socket_fd < 0 )
    {
        printf ( "ERROR opening socket" );
        exit ( 1 );
    }

    /* clear all garbage from serv_addr */
    bzero ( ( char * ) &serv_addr, sizeof ( serv_addr ) );

    /* configure setting for serv_addr */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons ( *hpcd_cli_setting.port );

    /* configure the socket server addr */
    printf ( "Binding socket\n" );
    if ( bind ( hpcd_server_socket_fd, ( struct sockaddr * ) &serv_addr,
                sizeof ( serv_addr ) ) < 0 )
    {
        printf ( "ERROR on binding" );
        exit ( 1 );
    }

    /* start listening on socket */
    printf ( "Starting to Listen %d\n", HTTP_REQUEST );
    listen ( hpcd_server_socket_fd, 5 );

    clilen = sizeof ( cli_addr );

    /* Main Server Loop */
    while ( 1 )
    {
        /* Start Accepting connections */
        printf ( "Accepting new connection\n" );
        newsockfd = ( int * ) malloc ( sizeof ( int ) );
        *newsockfd = accept ( hpcd_server_socket_fd,
                              ( struct sockaddr * ) &cli_addr,
                              &clilen );

        /* Create independent threads each of which will execute function */
        pthread_create ( &thread1, &attr, hpcd_server_handle_connection, ( void * ) newsockfd );

        /* Found a new connection */
        printf ( "Accepted!! %d\n", ++ctr );
        if ( *newsockfd < 0 )
        {
            printf ( "ERROR on accept" );
            exit ( 1 );
        }
    }

}

int hpcd_server_handle_on_url ( http_parser *parser, const char *at, size_t length )
{
    int n,
        newsockfd = * ( ( int * ) parser->data );
    hpcd_hash_item *itm;
    char *key = ( char * ) malloc ( ( int ) length * sizeof ( char ) );
    strncpy ( key, at, length );

    printf ( "Url: %.*s\n", ( int ) length, key );

    itm = hpcd_hash_item_fetch ( hpcd_hash_table_plain, key );


    if ( itm == NULL )
    {
        printf ( "Not Found\n" );
        itm = hpcd_hash_item_fetch ( hpcd_hash_table_plain, "404" );
    }

    n = write ( newsockfd, itm->content, itm->length );

    if ( n < 0 )
    {
        printf ( "ERROR writing to socket" );
        exit ( 1 );
    }



    return 0;
}

void *hpcd_server_handle_connection ( void *arg )
{
    char buffer[80 * 1024];
    int n,
        newsockfd = * ( ( int * ) arg );
    free ( arg );

    http_parser_settings settings;

    memset ( &settings, 0, sizeof ( settings ) );
    settings.on_url = hpcd_server_handle_on_url;


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
        printf ( "ERROR reading from socket" );
        exit ( 1 );
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
