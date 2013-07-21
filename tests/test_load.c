#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../load.h"

int main()
{

    void *cnt;

    headers httphds =
    {
        HTTP_STATUS_404,
        "text/html",
        "A",
        NULL,
        "gzip"
    };

    cnt  = load_httpcontent_memory ( &httphds,
                                     NULL, ( size_t ) 1 );

    return 0;

}