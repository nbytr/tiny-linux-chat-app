#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"

int process_arguments (int argc, char *argv[], CliArguments *out)
{
  // Check for the exceptional --help argument first
  if (argc == 2) {
    if (strcmp (argv[1], "--help") == 0) {
      printf ("%s\n  %s\n  %s\n  %s\n\n",
          "Tiny Linux Chat App Server command line arguments:",
          "--port\tSet the port to listen on [DEFAULT is 4950]",
          "--max-conn\tSet the max amount of clients that can be connected at the same time [DEFAULT is 128]",
          "--help\tPrint this guide"
          );
      exit (0);
    }
  }

  // Check for argument,value pairs
  if ((argc + 1) % 2 != 0) {
    return -1;
  }

  char *result;
  for (int x = 1; x < argc; x+=2) {
    if (strcmp (argv[x], "--port") == 0) {
      long port = 0;
      port = strtol (argv[x+1], &result, 10);
      if (*result != '\0')
        return -1;
      out->port = (uint16_t)port;
      continue;
    }
    if (strcmp (argv[x], "--max-conn") == 0) {
      long max_conn = 0;
      max_conn = strtol (argv[x+1], &result, 10);
      if (*result != '\0')
        return -1;
      out->max_connections = (int)max_conn;
      continue;
    }
    printf ("Unrecognised command: %s\n", argv[x]);
    return -1;
  }

  return 0;
}
