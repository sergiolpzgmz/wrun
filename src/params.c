#include <stdio.h>
#include <getopt.h>

#include "../include/params.h"

#define PROGRAM_NAME "wrun"
#define TRY_HELP_MSG "Try 'wrun --help' for more info"

static struct option const longopts[] = {
    {"kill", no_argument, NULL, 'k'},
    {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'V'},
    {NULL, 0, NULL, 0}};

void usage()
{
    printf("Usage: %s [OPTION]... -p PORT\n", PROGRAM_NAME);

    fputs(
        "Display the process or processes bound to a TCP port, or terminate them.\n"
        "\n",
        stdout);

    fputs(
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "\n",
        stdout);

    fputs(
        "  -p, --port=PORT     specify the TCP port to operate on (mandatory)\n"
        "  -k, --kill          terminate the process using the specified port\n"
        "\n",
        stdout);

    fputs(
        "  -h, --help          display this help and exit\n"
        "  -V, --version       output version information and exit\n",
        stdout);
}

int run(int argc, char *argv[])
{
    int optc;
    int kill_flag = 0;
    int port_flag = 0;
    char *port;

    while ((optc = getopt_long(argc, argv, "kp:", longopts, NULL)) != -1)
    {
        switch (optc)
        {
        case 'k':
            kill_flag = 1;
            break;
        case 'p':
            port_flag = 1;
            port = optarg;
            break;
        case '?':
            if (optopt == 'p')
            {
                fprintf(stderr, "%s\n", TRY_HELP_MSG);
                return 1;
            }
            fprintf(stderr, "wrun: invalid option or missing argument\n%s\n", TRY_HELP_MSG);
            return 1;
        }
    }

    if (!port_flag)
    {
        fprintf(stderr, "wrun: missing required option '-p'\n%s\n", TRY_HELP_MSG);
        return 1;
    }

    return 0;
}