#ifndef PARAMS_H
#define PARAMS_H

#include "proc.h"

static void usage();
int run(int argc, char *argv[]);
static int parse_and_check_port(char *input_port);
static void show_process_output_res(Process_output_list process_result_list);

#endif