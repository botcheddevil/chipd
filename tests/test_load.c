#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../load.h"

int main()
{

    void *cnt;
    char *content = "marry had a little lamb, little lamb, little lamb";

    headers httphds =
    {
        HTTP_STATUS_404,
        "text/html",
        "A",
        NULL,
        "gzip"
    };

    printf ( "String : %s\n", content );

    cnt  = hpcd_load_httpcontent_memory ( &httphds,
                                          ( void * ) content, strlen ( content ) );

    return 0;

}
