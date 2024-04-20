#include "executor_utils.h"

void print_execution_unit(ExecUnit unit) {
  printf("Command: %s\n", unit.command.executable);
  printf("Input FD: %d\n", unit.input_fd);
  printf("Output FD: %d\n", unit.output_fd);
}
