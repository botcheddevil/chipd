#include "server.h"

void hpcd_server_init() {

    /* Lets create a socket */
    printf ( "Creating socket\n" );
    hpcd_server_socket_fd = socket ( AF_INET, SOCK_STREAM, 0 );

    /**
     * If we are unable to create socket
     * then raise an error
     */
    if ( hpcd_server_socket_fd < 0 )
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
    if ( bind ( hpcd_server_socket_fd, ( struct sockaddr * ) &serv_addr,
                sizeof ( serv_addr ) ) < 0 )
    {
        error ( "ERROR on binding" );
    }

    /* start listening on socket */
    printf ( "Starting to Listen %d\n", HTTP_REQUEST );
    listen ( hpcd_server_socket_fd,5 );

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
        pthread_create ( &thread1, &attr, handle_connection, ( void * ) newsockfd );

        /* Found a new connection */
        printf ( "Accepted!! %d\n", ++ctr );
        if ( *newsockfd < 0 )
        {
            error ( "ERROR on accept" );
        }
    }

}

int hpcd_server_handle_on_url ( http_parser *parser, const char *at, size_t length )
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

void *hpcd_server_handle_connection ( void *arg )
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
