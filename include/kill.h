#ifndef KILL_H
#define KILL_H

static int check_process_access(const pid_t pid);
int send_kill_signal(const pid_t pid, const int signal);

#endif