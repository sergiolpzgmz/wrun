#include "../include/utils.h"

int check_tcp_port_range(int *port)
{
    if (*port)
    {
        int low = 1;
        int high = 65535;
        if (*port >= low && *port <= high)
        {
            return 1;
        }
    }
    return 0;
}