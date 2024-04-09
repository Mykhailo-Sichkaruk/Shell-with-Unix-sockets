#include "parse.c"
#include <slog.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const char *default_host_str = "0.0.0.0";
static const uint32_t default_host_address = 0x00000000;
static const uint16_t default_port = 9999;
static const net_socket default_socket = {default_port, default_host_address};

void start_server(net_socket server_addr) {
  struct addrinfo hints, *res;
  int status;
  // char ipstr[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;
  char host_str[INET_ADDRSTRLEN];
  char port_stt[7];
  snprintf(port_stt, sizeof(port_stt), "%d", server_addr.port);
  inet_ntop(AF_INET, &server_addr.address, host_str, INET_ADDRSTRLEN);
  if ((status = getaddrinfo(host_str, port_stt, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  }
  printf("IP addresses for %s:\n\n", host_str);
  char str[] = "ls -l\nls -a\n\0";
  parse(str);
}

void server(server_options options) {
  slog_info("Starting server...");
  switch (options.tag) {
  case PORT:
    printf("Port: %d\n", options.port);
    break;
  case SOCKET:
    printf("Socket: %d\n", options.socket.port);
    break;
  case UNINITIALIZED_SERVER_OPTIONS:
    fprintf(stderr,
            "Server options are uninitialized. Using default socket - %s:%d\n",
            default_host_str, default_port);
    break;
  }

  start_server(options.socket);
}
