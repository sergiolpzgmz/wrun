#define _POSIX_C_SOURCE 200809L

#include "../include/params.h"

int main(int argc, char *argv[])
{
    int output_code = run(argc, argv);
    return output_code;
}
