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
    serv_addr.sin_port = htons ( atoi ( hpcd_cli_setting.port ) );

    /* configure the socket server addr */
    printf ( "Binding socket %s\n", hpcd_cli_setting.port );
    if ( (bind ( hpcd_server_socket_fd, ( struct sockaddr * ) &serv_addr,
                sizeof ( serv_addr ) ) ) < 0 )
    {
        do { perror("bind"); exit(EXIT_FAILURE); } while (0);
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
            printf ( "ERROR on accept\n" );
            exit ( 1 );
        }
    }

}

int hpcd_server_handle_on_url ( http_parser *parser, const char *at, size_t length )
{
    hpcd_server_http_request *request_container = parser->data;
    request_container->url = ( char * ) malloc ( ( ( int ) length * sizeof ( char ) ) );

    strncpy ( request_container->url, at, length );

    printf("Found URL: %s\n", request_container->url);

    return 0;
}

int hpcd_server_handle_on_headers_complete ( http_parser *parser )
{
    printf("Headers complete\n");

    return 0;
}

int hpcd_server_handle_on_header_field ( http_parser *parser, const char *at, size_t length )
{
    printf("On header field : %.*s\n", (int) length, at);

    return 0;
}

int hpcd_server_handle_on_header_value ( http_parser *parser, const char *at, size_t length )
{
    printf("On header value : %.*s\n", (int) length, at);

    return 0;
}

int hpcd_server_handle_on_message_complete ( http_parser *parser ) {

    hpcd_server_http_request *request_container = parser->data;
    hpcd_hash_item *itm;
    printf("Message has completed\n");
    //printf ( "Url: %s\n", request_container->url );

    itm = hpcd_hash_item_fetch ( hpcd_hash_table_plain, request_container->url );

    if ( itm == NULL )
    {
        printf ( "Not Found\n" );
        itm = hpcd_hash_item_fetch ( hpcd_hash_table_plain, "404" );
    }

    if ( write ( *request_container->sock_fd, itm->content, itm->length ) < 0 )
    {
        do { perror("write"); exit(EXIT_FAILURE); } while (0);
    }

    close(*request_container->sock_fd);

    request_container->complete = 1;

    printf("request_container->complete %d\n", request_container->complete);

    return 0;
}

void *hpcd_server_handle_connection ( void *arg )
{
    char buffer[80 * 1024];
    int n,
        newsockfd = * ( ( int * ) arg );
    free ( arg );


    /** Set time limit on execution of thread **/

    clock_t begin, end;
    double time_spent = 0;

    begin = clock();


    http_parser_settings settings;
    hpcd_server_http_request *request_container = (hpcd_server_http_request *) malloc ( sizeof ( hpcd_server_http_request ) );
    request_container->complete = 0;


    memset ( &settings, 0, sizeof ( settings ) );
    settings.on_url = hpcd_server_handle_on_url;
    settings.on_message_complete = hpcd_server_handle_on_message_complete;
    settings.on_headers_complete = hpcd_server_handle_on_headers_complete;
    settings.on_header_field = hpcd_server_handle_on_header_field;
    settings.on_header_value = hpcd_server_handle_on_header_value;

    /* Clear the buffer */
    bzero ( buffer, 80 * 1024 );

    http_parser *parser = malloc ( sizeof ( http_parser ) );
    http_parser_init ( parser, HTTP_REQUEST );
    request_container->sock_fd = &newsockfd;
    parser->data = request_container;

    while(!request_container->complete) {

        /* Reading from buffer */
        //printf ( "Reading from buffer: %d\n ", request_container->complete );
        n = recv ( newsockfd, buffer, 80 * 1024, 0 );

        if ( n < 0 )
        {
            printf ( "ERROR reading from socket %d", n );
            exit ( 1 );
        }

        //printf("captured n %d\n", n);

        size_t nparsed = http_parser_execute ( parser, &settings, buffer, n );

        if ( nparsed != ( size_t ) n )
        {
            fprintf ( stderr,
                      "Error: %s (%s)\n",
                      http_errno_description ( HTTP_PARSER_ERRNO ( parser ) ),
                      http_errno_name ( HTTP_PARSER_ERRNO ( parser ) ) );
        }

        bzero ( buffer, n );


        /** Thread execution time **/

        end = clock();
        if (((double)(end - begin) / CLOCKS_PER_SEC) > 60) {
            printf("Request timed out\n");
            close(*request_container->sock_fd);
            break;
        }
    }

    printf("Loop Closed\n");

    return NULL;
}
