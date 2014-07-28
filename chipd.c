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

#include "chipd.h"

hpcd_cli_settings hpcd_cli_setting;

int main ( int argc, char *argv[] )
{

    /**
     * Set default options
     */

    hpcd_cli_setting.verbose = 0;
    hpcd_cli_setting.hash_algorithm = HPCD_HASH_GUESS;
    getcwd ( hpcd_cli_setting.directory, sizeof ( hpcd_cli_setting.directory ) );
    hpcd_cli_setting.port = "80";
    hpcd_cli_setting.filetypes = "html,jpg,jpeg,png,gif,txt,pdf";
    hpcd_cli_setting.help = 0;
    hpcd_cli_setting.packet_cache = 1;
    hpcd_cli_setting.gzip_content = 1;
    hpcd_cli_setting.deflate_content = 1;


    /**
     * Read options provided
     */

    int option_index, option_char;

    static struct option long_options[] =
    {
        {"verbose",         no_argument,       &hpcd_cli_setting.verbose, 1},
        {"packet-cache",    no_argument,       &hpcd_cli_setting.packet_cache, 1},
        {"no-packet-cache", no_argument,       &hpcd_cli_setting.packet_cache, 0},
        {"help",            no_argument,       &hpcd_cli_setting.help, 1},
        {"gzip",            no_argument,       &hpcd_cli_setting.gzip_content, 1},
        {"deflate",         no_argument,       &hpcd_cli_setting.deflate_content, 1},
        {"no-gzip",         no_argument,       &hpcd_cli_setting.gzip_content, 0},
        {"no-deflate",      no_argument,       &hpcd_cli_setting.deflate_content, 0},
        {"no-compress",     no_argument,       0, 0},
        {"directory",       required_argument, 0, 'd'},
        {"hash",            required_argument, 0, 's'},
        {"port",            required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    while ( -1 != ( option_char = getopt_long ( argc, argv, "d:s:p:",
                                  long_options, &option_index ) ) )
    {
        switch ( option_char )
        {
        case 0:
            if ( long_options[option_index].flag != 0 )
            {
                break;
            }
            if ( strcmp ( long_options[option_index].name, "no-compress" ) == 0 )
            {
                hpcd_cli_setting.gzip_content = 0;
                hpcd_cli_setting.deflate_content = 0;
            }
            break;

        case 'p':
            hpcd_cli_setting.port = optarg;
            break;

        case 'd':
            strcpy ( hpcd_cli_setting.directory, optarg );
            break;

        case 's':
            hpcd_cli_setting.hash_algorithm = optarg;
            break;

        case '?':
            /* getopt_long already printed an error message. */
            printf ( "Invalid option(s): Cant start chipd\n" );
            exit ( 0 );
            break;

        default:
            abort ();
        }
    }

    hpcd_register_signal_handlers();

    /**
     * Load files in a directory
     */
    hpcd_hash_table_plain = hpcd_hash_table_create ( &hpcd_hash_xor,
                            hpcd_load_directory_filecount ( hpcd_cli_setting.directory,
                                    hpcd_cli_setting.filetypes ) );

    hpcd_load_directory ( hpcd_cli_setting.directory, hpcd_hash_table_plain,
                          strlen ( hpcd_cli_setting.directory ) );
    hpcd_hash_item_insert ( hpcd_hash_table_plain, hpcd_load_notfound() );

    hpcd_server_init();

    return 0;
}
