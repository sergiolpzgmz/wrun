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

struct tcp_results get_sockets_by_port(const char *file_name, const char *port);

#endif