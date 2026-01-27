#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "../include/proc.h"

#define PROC_PATH "/proc"
#define COMM_PATH "/comm"
#define TCP_ROUTE "/proc/net/tcp"
#define TCP_V6_ROUTE "/proc/net/tcp6"
#define LISTEN_STATUS 10

/**
 * Parses network socket information from a /proc/net/tcp style file.
 * * @param file_name Path to the proc file (e.g., /proc/net/tcp).
 * @param port The target port to filter by (in decimal string format).
 * @return A tcp_results struct containing the pointer to the array and the count.
 * If no matches or an error occurs, the array pointer will be NULL.
 * @note THE CALLER IS RESPONSIBLE FOR FREEING THE 'array' FIELD WITHIN THE RETURNED STRUCT.
 */
static struct tcp_results get_sockets_by_port(const char *file_name, const char *port)
{

    Tcp_dinamic_list tcp_list = {NULL, 0, 0};
    struct tcp_results results = {NULL, 0};

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        return results;
    }

    char buffer[256];
    // Skip the header line
    if (fgets(buffer, sizeof(buffer), file) == NULL)
    {
        fclose(file);
        return results;
    }

    struct tcp_info tcp_data;
    int target_port = (int)strtol(port, NULL, 10);

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (sscanf(buffer,
                   "%*d: %*X:%X %*X:%*X %X %*X:%*X %*X:%*X %*X %*d %*d %lu",
                   &tcp_data.local_port,
                   &tcp_data.status,
                   &tcp_data.inode) == 3)
        {
            if (tcp_data.local_port == target_port && tcp_data.status == LISTEN_STATUS)
            {
                dl_add(tcp_list, tcp_data);
            }
        }
    }

    fclose(file);

    if (tcp_list.count == 0)
    {
        free(tcp_list.items);
        return results;
    }

    results.array = tcp_list.items;
    results.count = tcp_list.count;

    return results;
}

/**
 * Searches through the /proc filesystem to find all PIDs that own a specific socket inode.
 * @param inode The target socket inode number to look for.
 * @return A Pid_list struct containing a dynamically allocated array of PIDs and the count.
 * If no processes are found or an error occurs, the items field will be NULL.
 * @note THE CALLER IS RESPONSIBLE FOR FREEING THE 'items' FIELD WITHIN THE RETURNED STRUCT.
 */
static Pid_list get_pids_by_inode(const long inode)
{

    Pid_list pid_res = {NULL, 0, 0};

    char target_socket_str[64];
    snprintf(target_socket_str, sizeof(target_socket_str), "socket:[%lu]", inode);

    DIR *proc_folder = opendir(PROC_PATH);
    if (proc_folder == NULL)
    {
        free(pid_res.items);
        pid_res.items = NULL;
        return pid_res;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_folder)) != NULL)
    {
        if (!isdigit(entry->d_name[0]))
            continue;

        char fd_folder_path[512];
        snprintf(fd_folder_path, sizeof(fd_folder_path), "%s/%s/fd", PROC_PATH, entry->d_name);
        DIR *fd_folder = opendir(fd_folder_path);

        if (fd_folder == NULL)
            continue;

        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_folder)) != NULL)
        {
            if (!isdigit(fd_entry->d_name[0]))
                continue;

            char link_path[1024];
            char link_value[512] = {0};

            snprintf(link_path, sizeof(link_path), "%s/%s", fd_folder_path, fd_entry->d_name);
            ssize_t len = readlink(link_path, link_value, sizeof(link_value) - 1);

            if (len > 0)
                link_value[len] = '\0';

            if (strcmp(target_socket_str, link_value) == 0)
            {
                dl_add(pid_res, atoi(entry->d_name));
                break;
            }
        }
        closedir(fd_folder);
    }
end:
    closedir(proc_folder);
    if (pid_res.count == 0)
    {
        free(pid_res.items);
        pid_res.items = NULL;
    }

    return pid_res;
}

/**
 * Displays information about a process listening on a specific TCP port.
 * @param pid The process ID to query for information.
 * @param port The TCP port number that the process is using.
 */
static void show_process_info(const int pid, const char *port, int tcp_v6)
{
    char comm_full_path[256];
    char res_process[256] = "Unknown";
    char *tcp_res = "tcp";

    snprintf(comm_full_path, sizeof(comm_full_path), "%s/%d%s", PROC_PATH, pid, COMM_PATH);

    FILE *file = fopen(comm_full_path, "r");
    if (file)
    {
        if (fgets(res_process, sizeof(res_process), file) != NULL)
        {
            res_process[strcspn(res_process, "\n")] = 0;
        }
        fclose(file);
    }

    if (tcp_v6 == 1)
    {
        tcp_res = "tcp6";
    }

    printf("%d %s %s %s\n", pid, res_process, port, tcp_res);
}

/**
 * Processes TCP socket results to find and display process information.
 * @param res_tcp A tcp_results struct containing socket information to process.
 * @param port The port number being searched for.
 * @param tcp_v6 Flag indicating if this is for IPv6 (1) or IPv4 (0).
 */
static void process_tcp_results(const struct tcp_results res_tcp, const char *port, int tcp_v6)
{
    if (res_tcp.array != NULL)
    {
        for (int i = 0; i < res_tcp.count; i++)
        {
            Pid_list pid_res = get_pids_by_inode(res_tcp.array[i].inode);
            if (pid_res.items != NULL)
            {
                for (size_t j = 0; j < pid_res.count; j++)
                {
                    show_process_info(pid_res.items[j], port, tcp_v6);
                }
                free(pid_res.items);
            }
        }
    }
}

/**
 * Main function to find and display processes listening on a specific TCP port.
 * @param port The port number to search for processes listening on.
 */
void run_process_finder(const char *port)
{
    struct tcp_results res_tcp = get_sockets_by_port(TCP_ROUTE, port);
    struct tcp_results res_tcp_v6 = get_sockets_by_port(TCP_V6_ROUTE, port);

    process_tcp_results(res_tcp, port, 0);
    process_tcp_results(res_tcp_v6, port, 1);

    free(res_tcp.array);
    free(res_tcp_v6.array);
}