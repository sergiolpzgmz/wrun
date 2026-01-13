#include <stdio.h>
#include <stdlib.h>

#define TCP_ROUTE "/proc/net/tcp"
#define TCP_V6_ROUTE "/proc/net/tcp6"

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
            if (tcp_data.local_port == target_port)
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