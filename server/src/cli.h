#pragma once

#include <stdint.h>

typedef struct
{
  int max_connections;
  uint16_t port;
} CliArguments;

int process_arguments (int argc, char *argv[], CliArguments *out);
