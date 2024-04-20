#include "parse_utils.h"
#include <malloc.h>
#include <slog.h>

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
  slog("File: %s, Redirection: %s", fileRedirection.file, redirectionType);
}

void print_command(Command command) {
  slog("Executable: %s", command.executable);
  slog("Args (%zu): ", command.args_length);
  for (size_t i = 0; i < command.args_length; ++i) {
    slog("\"%s\"", command.args[i]);
  }
}

void print_pipeline(Pipeline pipeline) {
  slog("Pipeline: %s", pipeline.str);
  print_command(pipeline.command);
  const char *redirectionType = "";
  switch (pipeline.redirection) {
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
  slog("Redirection: %s", redirectionType);
}

void print_component(SequenceComponent redirection) {
  switch (redirection.type) {
  case FILE_TYPE:
    slog("SequenceComponent: FILE_TYPE");
    print_file(redirection.component.file);
    break;
  case PIPELINE_TYPE:
    slog("SequenceComponent: PIPELINE_TYPE");
    print_pipeline(redirection.component.pipeline);
    break;
  }
}

void print_sequence(Sequence sequence) {
  slog("Sequence with %zu redirection(s):", sequence.redirection_length);
  slog("Sequence: %s", sequence.str);
  for (size_t i = 0; i < sequence.redirection_length; ++i) {
    slog("Redirection %zu:", i + 1);
    print_component(sequence.component[i]);
  }
}

void print_line(Line line) {
  slog("Line with %zu sequence(s):", line.sequence_length);
  for (size_t i = 0; i < line.sequence_length; ++i) {
    slog("Sequence %zu:", i + 1);
    print_sequence(line.sequence[i]);
  }
}

void free_command(Command *cmd) {
  if (cmd->args) {
    free(cmd->args); // Assuming args were allocated as a single block
  }
}

void free_sequence_component(SequenceComponent *component) {
  if (component->type == PIPELINE_TYPE) {
    free_command(&component->component.pipeline.command);
  } else if (component->type == FILE_TYPE) {
  }
}

void free_sequence(Sequence *sequence) {
  if (sequence->component) {
    for (size_t i = 0; i < sequence->redirection_length; ++i) {
      free_sequence_component(&sequence->component[i]);
    }
    free(sequence->component);
  }
}

void free_line(Line *line) {
  if (line->sequence) {
    for (size_t i = 0; i < line->sequence_length; ++i) {
      free_sequence(&line->sequence[i]);
    }
    free(line->sequence);
  }
}

void free_parser(Parser *parser) {
  if (parser->lines) {
    for (size_t i = 0; i < parser->lines_length; ++i) {
      free_line(&parser->lines[i]);
    }
    free(parser->lines);
  }
}
