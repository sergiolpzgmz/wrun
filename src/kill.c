#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <errno.h>
#include <stdio.h>

#include "../include/kill.h"

static int check_process_access(const pid_t pid)
{
    /*
     If we send 0, kill() performs error checking but does not send a signal
     This is how we check if the process exists at this point and if we have permission to terminate it.
     https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-kill-send-signal-process
    */
    if (kill(pid, 0) != 0)
    {
        return 1;
    }
    return 0;
}

int send_kill_signal(const pid_t pid, const int signal)
{
    if (check_process_access(pid) == 1)
    {
        return 1;
    }
    kill(pid, signal);
    return 0;
}