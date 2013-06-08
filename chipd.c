
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

int sockfd;

void *handle_connection( void *arg );
item *hash_table[255] = {0};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("received SIGINT\n");
        close(sockfd);
    }
}

int main(int argc, char *argv[])
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
    sigemptyset( &a.sa_mask );
    sigaction( SIGINT, &a, NULL );

    /* Create detached thread attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /**
     * If number of arguments are less than two then
     * then raise an error
     */
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }


    /**
     * --------------------
     * Load files to memory
     * --------------------
     */

    FILE *fp;
    /** Get File Length */
    struct stat info;
    const char *filename = "file_to_read.gif";
    const char *separator = "/";
    unsigned index;
    stat(filename, &info);

    /** Get File Content */
    char *content = (char *)malloc(info.st_size * sizeof(char));
    fp = fopen(filename, "rb");
    fread(content, info.st_size, 1, fp);
    fclose(fp);

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

    unsigned long RESPONSE_LENGTH = strlen(response_header_part1) + 10 + strlen(response_header_part3) + (unsigned long)info.st_size;
    char *response = (char *)malloc(RESPONSE_LENGTH);

    /** Concat all string into response **/
    snprintf (response, (size_t)RESPONSE_LENGTH, "%s%lu%s", response_header_part1, (unsigned long)info.st_size, response_header_part3);

    /*
    printf("FILE SIZE: %lu\n", (unsigned long)info.st_size);
    printf("HEADER LENGTH %zd\n", strlen(response));
    printf("HEADER  : \n%s\n", response);
    printf("TOTAL LENGTH: %ld\n", RESPONSE_LENGTH);
    printf("RESPONSE ADDRESS: %x\n", (void *)response);
    printf("CONTENT ADDRESS: %x\n", response + ((int)strlen(response) * sizeof(char *)));
    */
    //memcpy (response + (int)strlen(response), content, 30);
    memcpy (response + (int)strlen(response), content, info.st_size);

    //printf("NEW HEADER  : \n%s\n", response);

    /** Generate Key **/
    char *key[(int)strlen(filename)+1];


    strcat(key, separator);
    strcat(key, filename);

    printf(" Key %s\n", key);

    /** Calculate index **/
    index = xor_hash(key, strlen(key));

    printf(" HashValue of filename %d\n", index);

    hash_table[index] = (item *) malloc(sizeof(item));

    hash_table[index]->content = response;
    hash_table[index]->key = key;
    hash_table[index]->length = (size_t)RESPONSE_LENGTH;




    /* Lets create a socket */
    printf("Creating socket\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /**
     * If we are unable to create socket
     * then raise an error
     */
    if (sockfd < 0)
        error("ERROR opening socket");

    /* clear all garbage from serv_addr */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    /* get the port number from argument and convert it to string */
    portno = atoi(argv[1]);

    /* configure setting for serv_addr */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* configure the socket server addr */
    printf("Binding socket\n");
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    /* start listening on socket */
    printf("Starting to Listen %d\n", HTTP_REQUEST);
    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    /* Main Server Loop */
    while (1)
    {
        /* Start Accepting connections */
        printf("Accepting new connection\n");
        newsockfd = (int *)malloc(sizeof(int));
        *newsockfd = accept(sockfd,
                            (struct sockaddr *) &cli_addr,
                            &clilen);

        /* Create independent threads each of which will execute function */
        pthread_create( &thread1, &attr, handle_connection, (void *)newsockfd);

        /* Found a new connection */
        printf("Accepted!! %d\n", ++ctr);
        if (*newsockfd < 0)
            error("ERROR on accept");
    }

    return 0;
}

int on_url(http_parser* parser, const char* at, size_t length)
{
    int n,
        newsockfd = *((int *)parser->data);
    unsigned int index;

    printf("Found a URL\n");

    index = xor_hash((void *)at, length);

    printf(" HashValue of filename %d\n", index);

    n = write(newsockfd, hash_table[index]->content, hash_table[index]->length);

    if (n < 0) error("ERROR writing to socket");

    printf("Url: %.*s\n", (int)length, at);

    return 0;
}

void *handle_connection(void *arg)
{
    char buffer[80 * 1024];
    int n,
        newsockfd = *((int *)arg);
    free(arg);

    http_parser_settings settings;

    memset(&settings, 0, sizeof(settings));
    settings.on_url = on_url;


    /* Clear the buffer */
    bzero(buffer, 80 * 1024);

    http_parser *parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = &newsockfd;

    /* Reading from buffer */
    printf("Reading from buffer\n");
    n = recv(newsockfd, buffer, 80 * 1024, 0);

    if (n < 0) error("ERROR reading from socket");

    size_t nparsed = http_parser_execute(parser, &settings, buffer, n);

    if (nparsed != (size_t)n)
    {
        fprintf(stderr,
                "Error: %s (%s)\n",
                http_errno_description(HTTP_PARSER_ERRNO(parser)),
                http_errno_name(HTTP_PARSER_ERRNO(parser)));
    }

    close(newsockfd);

    return NULL;
}
