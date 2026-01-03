#include <stdio.h>
#include <getopt.h>

#define PROGRAM_NAME "wrun"

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