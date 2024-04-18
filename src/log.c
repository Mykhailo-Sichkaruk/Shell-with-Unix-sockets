#include <slog.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *string_concat(char *str1, char *str2) {
  char *result = malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}

void init_logger(char *log_file) {
  slog_config_t cfg;
  char *log_path = string_concat("./logs/", log_file);
  slog_init(log_path, SLOG_FLAGS_ALL, 0);
  slog_config_get(&cfg);
  cfg.eColorFormat = SLOG_COLORING_FULL;
  cfg.nToFile = 1;
  cfg.nKeepOpen = 1;
  slog_config_set(&cfg);
  slog_info("Logger initialized. Lof file: %s", log_path);
  free(log_path);
}
