#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <slog.h>
static char *const program_name = "chell";

void help() {
  slog_warn("Usage: %s [-ilw] [file...]\n"
  "  -c  client mode\n"
  "  -s  server mode\n"
  "  -h  help page\n"
  "  -p  port (only in server mode)\n"
  "  -u  socket - ip:port (only in server mode)"
			, program_name);
  exit(EXIT_SUCCESS);
}
