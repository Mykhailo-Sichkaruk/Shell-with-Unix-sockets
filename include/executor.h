#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <errno.h>
#include <fcntl.h>
#include <parse.h>
#include <slog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum {
  EXEC_SUCCESS,
  EXEC_FAILURE,
} EXEC_RESULT_TYPE;

typedef struct {
  char *error_message;
  EXEC_RESULT_TYPE result;
} Exec_Result;

typedef struct ExecUnit {
  Command command;
  int input_fd;
  int output_fd;
} ExecUnit;

// Declare STDOUT as DEFAULT_OUTPUT
#define DEFAULT_OUTPUT 1
#define DEFAULT_INPUT 0

Exec_Result exec_sequence(Sequence *sequence, int output_fd);
Exec_Result exec_command(ExecUnit unit, int output_fd);
Exec_Result exec(Parser parser, int output_fd);
#include "executor_utils.h"
#endif // EXECUTOR_H
