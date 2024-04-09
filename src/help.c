#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static char *const program_name = "chell";

void help() {
  printf("Usage: %s [-ilw] [file...]\n", program_name);
  printf("  -c  client mode\n");
  printf("  -s  server mode\n");
  printf("  -h  help page\n");
  printf("  -p  port (only in server mode)\n");
  printf("  -u  socket - ip:port (only in server mode)\n");
  exit(EXIT_SUCCESS);
}
