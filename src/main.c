#include "client.c"
#include "help.c"
#include "log.c"
#include "server.c"
#include "types.h"
#include <slog.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  init_logger();
  options options = {UNINITIALIZED_CHELL_MODE,
                     {UNINITIALIZED_SERVER_OPTIONS, .port = 0}};
  int opt;

  while ((opt = getopt(argc, argv, "hcsu:")) != -1) {
    switch (opt) {
    case 'h':
      help();
      break;
    case 'c':
      options.mode = CLIENT_MODE;
      break;
    case 's':
      options.mode = SERVER_MODE;
      break;
    default:
      help();
      exit(EXIT_FAILURE);
    }
  }

  if (options.mode == CLIENT_MODE) {
    client();
  } else if (options.mode == SERVER_MODE) {
    server(options.server);
  } else {
    fprintf(stderr, "You must specify a mode - either SERVER or CLIENT.\n");
    help();
    exit(EXIT_FAILURE);
  }
}
