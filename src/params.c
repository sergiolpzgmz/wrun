#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#include "../include/params.h"
#include "../include/utils.h"
#include "../include/proc.h"
#include "../include/kill.h"

#define PROGRAM_NAME "wrun"
#define VERSION "0.1.0"
#define TRY_HELP_MSG "Try 'wrun --help' for more info"

static struct option const longopts[] = {
    {"kill", no_argument, NULL, 'k'},
    {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0}};

static void version()
{
    printf("%s v%s © 2026 by Sergio López Gómez\n", PROGRAM_NAME, VERSION);
}

static void usage()
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
        "  -v, --version       output version information and exit\n",
        stdout);
}

static int parse_and_check_port(char *input_port)
{
    if (input_port)
    {
        for (int i = 0; input_port[i] != '\0'; i++)
        {
            if (!isdigit(input_port[i]))
            {
                fprintf(stderr, "Invalid TCP port %s\n%s\n", input_port, TRY_HELP_MSG);
                return 1;
            }
        }

        int target_port = atoi(input_port);
        int valid_range = check_tcp_port_range(&target_port);

        if (!target_port || !valid_range)
        {
            fprintf(stderr, "Invalid TCP port %i\n%s\n", target_port, TRY_HELP_MSG);
            return 1;
        }
    }
    return 0;
}

static void show_process_output_res(Process_output_list process_result_list)
{
    printf("%-7s %-10s %-6s %-6s\n", "PID", "PROCESS", "PORT", "SOCKET");

    for (size_t i = 0; i < process_result_list.count; i++)
    {
        printf("%-7d %-10s %-6s %-6s\n",
               process_result_list.items[i].pid,
               process_result_list.items[i].process,
               process_result_list.items[i].port,
               process_result_list.items[i].tcp_version);
    }
}

int run(int argc, char *argv[])
{
    int optc;
    int kill_flag = 0;
    int port_flag = 0;
    char *port;

    while ((optc = getopt_long(argc, argv, "kp:hv", longopts, NULL)) != -1)
    {
        switch (optc)
        {
        case 'h':
            usage();
            return 0;
        case 'v':
            version();
            return 0;
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

    // Check if recived port is a valid TCP port
    int invalid_port = parse_and_check_port(port);
    if (invalid_port)
    {
        return 1;
    }

    Process_output_list process_result_list = run_process_finder(port);

    if (process_result_list.count == 0)
    {
        fprintf(stderr, "wrun: %s: no process found\n", port);
        return 1;
    }

    if (kill_flag)
    {
        pid_t pid = 0;
        for (size_t i = 0; i < process_result_list.count; i++)
        {
            pid = process_result_list.items[i].pid;
            if (pid > 0)
            {
                if (send_kill_signal(pid, SIGTERM) == 1)
                {
                    fprintf(stderr, "wrun: failed to terminate process %d\n", pid);
                }
                else
                {
                    fprintf(stdout, "Killed process %d (%s)\n", pid, process_result_list.items[0].process);
                }
            }
        }

        free(process_result_list.items);
        return 0;
    }

    show_process_output_res(process_result_list);

    free(process_result_list.items);
    return 0;
}