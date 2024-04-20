#include <arpa/inet.h>
#include <slog.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_PORT 9999
#define DEFAULT_HOST "0.0.0.0"

void client(const char *host, int port) {
  int client_socket;
  struct sockaddr_in server_addr;
  char buffer[1024];

  // Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  // Set up server address structure
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
    perror("Invalid address or address not supported");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  slog_info("Connected to %s:%d", host, port);

  // Loop to send user input to the server and read responses
  while (true) {
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
      perror("Failed to receive data");
      break;
    }

    buffer[bytes_received] = '\0';
    printf("%s", buffer);
    fflush(stdout);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      perror("Error reading input");
      break;
    }

    if (strcmp(buffer, "quit\n") == 0) { // Break loop if 'quit' is entered
      break;
    }

    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
      perror("Failed to send data");
      break;
    }

    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
      perror("Failed to receive data");
      break;
    }

    buffer[bytes_received] = '\0';
    printf("%s", buffer);
  }

  // Close the socket when done
  close(client_socket);
}
