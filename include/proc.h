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

struct pid_results
{
    int *pid_array;
    int count;
};

static struct tcp_results get_sockets_by_port(const char *file_name, const char *port);
static struct pid_results get_pids_by_inode(const long inode);
static void show_process_info(const int pid, const char *port);
static void process_tcp_results(const struct tcp_results res_tcp, const char *port);
void run_process_finder(const char *port);

#endif