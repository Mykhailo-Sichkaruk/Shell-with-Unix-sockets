#include "types.h"
#include <arpa/inet.h>
#include <executor.h>
#include <malloc.h>
#include <netdb.h>
#include <parse.h>
#include <pwd.h>
#include <slog.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const char *default_host_str = "127.0.0.1";
static const int default_host_address = 0x00000000;
static const int default_port = 9999;
static const net_socket default_socket = {default_port, default_host_address};

// Function to create a custom prompt
char *create_prompt(char ending_char) {
  // Allocate a buffer for the prompt (ensure enough space)
  static const int prompt_size = 100;
  char *prompt = (char *)malloc(prompt_size * sizeof(char));
  if (!prompt) {
    perror("Failed to allocate memory for prompt");
    exit(EXIT_FAILURE);
  }

  // Get the current time
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  if (!timeinfo) {
    perror("Failed to get local time");
    free(prompt);
    exit(EXIT_FAILURE);
  }

  // Format the time as HH:MM
  char time_str[6]; // HH:MM
  strftime(time_str, sizeof(time_str), "%H:%M", timeinfo);

  // Get the username
  const char *username = getlogin();
  if (!username) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
      username = pw->pw_name;
    } else {
      username = "unknown";
    }
  }

  // Get the machine/hostname
  char hostname[56];
  gethostname(hostname, sizeof(hostname));

  // Construct the prompt
  snprintf(prompt, prompt_size, "%s %s@%s%c", time_str, username, hostname,
           ending_char);

  return prompt; // Caller is responsible for freeing the returned string
}

void handle_client(int client_fd) {
  char buffer[1024];
  ssize_t nread;
  const char *prompt = create_prompt('$');
  write(client_fd, prompt, strlen(prompt));

  while ((nread = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[nread] = '\0'; // Null-terminate whatever we read

    Parser parsed = parse(buffer); // Parse the command
    Exec_Result exec_result;

    exec_result = exec(parsed, client_fd);
    if (exec_result.result == EXEC_FAILURE) {
      // const char *error_msg = "Failed to execute command.\n";
      // write(client_fd, error_msg, strlen(error_msg));
    } else {
    }
    slog_info("Sending prompt");
    const char *prompt = create_prompt('$');
    write(client_fd, prompt, strlen(prompt));
  }

  if (nread == -1) {
    perror("read error");
    exit(EXIT_FAILURE);
  }

  close(client_fd); // Close the client socket at end of operation
}

void start_server(net_socket server_addr) {
  int server_fd;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  memset(&address, 0, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)&server_addr.address));
  address.sin_port = htons(server_addr.port);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  int listen_result = listen(server_fd, 10);
  if (listen_result < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  slog_info("Server listening on %s:%d", inet_ntoa(address.sin_addr),
            ntohs(address.sin_port));

  while (1) {
    int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }
    slog_info("Client connected");
    handle_client(client_fd);
    close(client_fd);
  }

  close(server_fd);
}

void server(server_options options) {
  slog_info("Starting server...");
  switch (options.tag) {
  case PORT:
    slog("Port: %d\n", options.port);
    break;
  case SOCKET:
    slog("Socket: %d\n", options.socket.port);
    break;
  case UNINITIALIZED_SERVER_OPTIONS:
    slog("Server options are uninitialized. Using default socket - %s:%d",
         default_host_str, default_port);
    options.socket = default_socket;
    break;
  }

  start_server(options.socket);
}
