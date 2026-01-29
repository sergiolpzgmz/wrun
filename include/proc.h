#include <stddef.h>

#ifndef PROC_H
#define PROC_H

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

struct process_output {
    int pid;
    char process[256];
    const char *port;
    char tcp_version[64];
};

typedef struct
{
    struct tcp_info *items;
    size_t count;
    size_t capacity;

} Tcp_dinamic_list;

typedef struct
{
    int *items;
    size_t count;
    size_t capacity;

} Pid_list;

typedef struct
{
    struct process_output *items;
    size_t count;
    size_t capacity;

} Process_output_list;

#define dl_add(list, item)                                                            \
    do                                                                                \
    {                                                                                 \
        if (list.count >= list.capacity)                                              \
        {                                                                             \
            if (list.capacity == 0)                                                   \
                list.capacity = 256;                                                  \
            else                                                                      \
                list.capacity *= 2;                                                   \
            void *new_ptr = realloc(list.items, list.capacity * sizeof(*list.items)); \
            if (new_ptr == NULL)                                                      \
                break;                                                                \
            list.items = new_ptr;                                                     \
        }                                                                             \
        list.items[list.count++] = item;                                              \
    } while (0)

static struct tcp_results get_sockets_by_port(const char *file_name, const char *port);
static Pid_list get_pids_by_inode(const long inode);
static struct process_output get_process_info(const int pid, const char *port, int tcp_v6);
static void process_tcp_results(const struct tcp_results res_tcp, const char *port, int tcp_v6, Process_output_list *process_output_list);
Process_output_list run_process_finder(const char *port);

#endif