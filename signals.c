#include "signals.h"

void hpcd_sigint_handler ( int signo )
{
    if ( signo == SIGINT )
    {
        printf ( "chipd: shutting down ...\n" );
        close ( hpcd_server_socket_fd );
    }
}

void hpcd_register_signal_handlers() {

    /* Handle SIGINT */
    struct sigaction a;
    a.sa_handler = hpcd_sigint_handler;
    a.sa_flags = 0;
    sigemptyset ( &a.sa_mask );
    sigaction ( SIGINT, &a, NULL );

}
