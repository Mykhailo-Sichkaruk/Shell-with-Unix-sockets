#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

enum chell_modes { SERVER_MODE, CLIENT_MODE, UNINITIALIZED_CHELL_MODE };
typedef enum chell_modes chell_mode;

struct net_socket {
  uint16_t port;
  uint32_t address;
};
typedef struct net_socket net_socket;

enum server_options_tag { PORT, SOCKET, UNINITIALIZED_SERVER_OPTIONS };
typedef enum server_options_tag server_options_tag;

struct server_options {
  server_options_tag tag;
  union {
    uint16_t port;
    net_socket socket;
  };
};
typedef struct server_options server_options;

struct options {
  chell_mode mode;
  server_options server;
};
typedef struct options options;

#endif // TYPES_H
