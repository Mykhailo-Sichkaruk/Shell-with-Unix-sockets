#include "../src/parse/parse.h"
#include <stdio.h>

void print_file(FileRedirection fileRedirection) {
  const char *redirectionType = "";
  switch (fileRedirection.redirection) {
  case INPUT_REDIRECTION:
    redirectionType = "Input Redirection";
    break;
  case OUTPUT_REDIRECTION:
    redirectionType = "Output Redirection";
    break;
  case PIPE_REDIRECTION:
    redirectionType = "Pipe Redirection";
    break;
  case NONE_REDIRECTION:
  default:
    redirectionType = "No Redirection";
    break;
  }
  printf("File: %s, Redirection: %s\n", fileRedirection.file, redirectionType);
}

void print_command(Command command) {
  printf("Executable: %s\n", command.executable);
  printf("Args (%zu): ", command.args_length);
  for (size_t i = 0; i < command.args_length; ++i) {
    printf("\"%s\"", command.args[i]);
    if (i < command.args_length - 1)
      printf(", ");
  }
  printf("\n");
}

void print_redirection(Redirection redirection) {
  switch (redirection.type) {
  case FILE_TYPE:
    printf("Redirection: FILE_TYPE\n");
    print_file(redirection.pipeline_file.file);
    break;
  case PIPELINE_TYPE:
    printf("Redirection: PIPELINE_TYPE\n");
    print_command(redirection.pipeline_file.pipeline.command);
    break;
  }
}

void print_sequence(Sequence sequence) {
  printf("Sequence with %zu redirection(s):\n", sequence.redirection_length);
  for (size_t i = 0; i < sequence.redirection_length; ++i) {
    printf("Redirection %zu:\n", i + 1);
    print_redirection(sequence.redirection[i]);
  }
}

void print_line(Line line) {
  printf("Line with %zu sequence(s):\n", line.sequence_length);
  for (size_t i = 0; i < line.sequence_length; ++i) {
    printf("Sequence %zu:\n", i + 1);
    print_sequence(line.sequence[i]);
  }
}
