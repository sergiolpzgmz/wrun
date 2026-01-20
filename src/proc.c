#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define PROC_PATH "/proc"
#define COMM_PATH "/comm"
#define TCP_ROUTE "/proc/net/tcp"
#define TCP_V6_ROUTE "/proc/net/tcp6"
#define LISTEN_STATUS 10

struct tcp_info
{
    unsigned int local_port;
    unsigned int status;
    unsigned long inode;
};

struct tcp_results
{
    struct tcp_info *array;
    int count;
};

/**
 * Parses network socket information from a /proc/net/tcp style file.
 * * @param file_name Path to the proc file (e.g., /proc/net/tcp).
 * @param port The target port to filter by (in decimal string format).
 * @return A tcp_results struct containing the pointer to the array and the count.
 * If no matches or an error occurs, the array pointer will be NULL.
 * @note THE CALLER IS RESPONSIBLE FOR FREEING THE 'array' FIELD WITHIN THE RETURNED STRUCT.
 */
struct tcp_results get_sockets_by_port(const char *file_name, const char *port)
{
    struct tcp_results final_res = {NULL, 0};

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        return final_res;
    }

    char buffer[256];
    // Skip the header line
    if (fgets(buffer, sizeof(buffer), file) == NULL)
    {
        fclose(file);
        return final_res;
    }

    int arr_capacity = 10;
    int current_found = 0;
    struct tcp_info *tcp_info_array = malloc(arr_capacity * sizeof(struct tcp_info));

    if (tcp_info_array == NULL)
    {
        fclose(file);
        return final_res;
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

                if (current_found >= arr_capacity)
                {
                    arr_capacity *= 2;
                    struct tcp_info *temp = realloc(tcp_info_array, arr_capacity * sizeof(struct tcp_info));
                    if (temp == NULL)
                    {
                        free(tcp_info_array);
                        fclose(file);
                        return final_res;
                    }
                    tcp_info_array = temp;
                }

                tcp_info_array[current_found] = tcp_data;
                current_found++;
            }
        }
    }

    fclose(file);

    if (current_found == 0)
    {
        free(tcp_info_array);
        return final_res;
    }

    final_res.array = tcp_info_array;
    final_res.count = current_found;

    return final_res;
}

struct pid_results
{
    int *pid_array;
    int count;
};

/**
 * Searches through the /proc filesystem to find all PIDs that own a specific socket inode.
 * * @param inode The target socket inode number to look for.
 * @return A pid_results struct containing a dynamically allocated array of PIDs and the count.
 * If no processes are found or an error occurs, the pid_array field will be NULL.
 * @note THE CALLER IS RESPONSIBLE FOR FREEING THE 'pid_array' FIELD WITHIN THE RETURNED STRUCT.
 */
struct pid_results get_pids_by_inode(const long inode)
{

    struct pid_results pid_res = {NULL, 0};
    int capacity = 5;

    pid_res.pid_array = malloc(capacity * sizeof(int));
    if (pid_res.pid_array == NULL)
        return pid_res;

    char target_socket_str[64];
    snprintf(target_socket_str, sizeof(target_socket_str), "socket:[%lu]", inode);

    DIR *proc_folder = opendir(PROC_PATH);
    if (proc_folder == NULL)
    {
        free(pid_res.pid_array);
        pid_res.pid_array = NULL;
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
            char link_value[512];

            snprintf(link_path, sizeof(link_path), "%s/%s", fd_folder_path, fd_entry->d_name);
            ssize_t len = readlink(link_path, link_value, sizeof(link_value) - 1);

            if (len != -1)
                link_value[len] = '\0';

            if (strcmp(target_socket_str, link_value) == 0)
            {
                if (pid_res.count >= capacity)
                {
                    capacity *= 2;
                    int *temp = realloc(pid_res.pid_array, capacity * sizeof(int));
                    if (temp == NULL)
                    {
                        closedir(fd_folder);
                        goto end;
                    }
                    pid_res.pid_array = temp;
                }
                pid_res.pid_array[pid_res.count] = atoi(entry->d_name);
                pid_res.count++;
                break;
            }
        }
        closedir(fd_folder);
    }
end:
    closedir(proc_folder);
    if (pid_res.count == 0)
    {
        free(pid_res.pid_array);
        pid_res.pid_array = NULL;
    }

    return pid_res;
}

/**
 * Displays information about a process listening on a specific TCP port.
 * @param pid The process ID to query for information.
 * @param port The TCP port number that the process is using.
 */
void show_process_info(const int pid, const char *port)
{
    char comm_full_path[256];
    char res_process[256] = "Unknown";

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
    printf("%d %s %s\n", pid, res_process, port);
}
