// Here should be parser for the shell commands
// `# ; < > | \`
// 1. Get lines = [Line]
// 2. get seqence of commands (splitted by `;`) = [Sequence]
// 3. get pipelines, splitted by redirections `> < |` = [Pipeline]
// 3.2 remove comments
// 4. get command [exec], get args = [Arg]
// 5. exec commands with args
// 6. handle redirections
// NOTE: We assume that line cannot be with symbol `\` extended
// NOTE: We assume that only one redirection can be in the line
// NOTE: Consider null=terminated strings
#include "parse.h"
#include <slog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Remove comments from a string by replacing the comment with a null
 * terminator
 * @param str The string to remove comments from
 * @return modified string
 */
char *remove_comments(char *str) {
  char *comment = strchr(str, '#');
  if (comment) {
    *comment = '\0';
  }
  return str;
}

Redirection_Type get_redirection(char redirection) {
  switch (redirection) {
  case '>':
    return OUTPUT_REDIRECTION;
  case '<':
    return INPUT_REDIRECTION;
  case '|':
    return PIPE_REDIRECTION;
  default:
    return NONE_REDIRECTION;
  }
}

char *trim(char *str) {
  while (*str == ' ') {
    str++;
  }
  while (str[strlen(str) - 1] == ' ') {
    str[strlen(str) - 1] = '\0';
  }
  return str;
}

SequenceComponent parse_file(char *file, Redirection_Type redirection) {
  SequenceComponent redirection_struct;
  redirection_struct.type = FILE_TYPE;
  redirection_struct.component.file.file = trim(file);
  redirection_struct.component.file.redirection = redirection;
  return redirection_struct;
}

// Pipeline parse_file
SequenceComponent parse_redirection(char *pipeline, Redirection_Type redirection) {
  Pipeline pipeline_struct;
  pipeline_struct.command.args_length = 0;
  pipeline_struct.command.args = NULL;
  pipeline_struct.redirection = redirection;
  char *separators = " ";
  char *token;
  char *rest = pipeline;
  while ((token = strtok_r(rest, separators, &rest)) != NULL) {
    if (pipeline_struct.command.args_length == 0) {
      pipeline_struct.command.executable = token;
    } else {
      pipeline_struct.command.args_length++;
      pipeline_struct.command.args = (char **)realloc(
          pipeline_struct.command.args,
          pipeline_struct.command.args_length * sizeof(char *));
      if (pipeline_struct.command.args == NULL) {
        slog_error("Failed to allocate memory for args. Closing program...");
        exit(EXIT_FAILURE);
      }
      pipeline_struct.command.args[pipeline_struct.command.args_length - 1] =
          token;
    }
  }

  SequenceComponent redirection_struct;
  redirection_struct.type = PIPELINE_TYPE;
  redirection_struct.component.pipeline = pipeline_struct;
  return redirection_struct;
}

Sequence parse_sequence(char *sequence) {
  const char *sequence_copy =
      strdup(sequence); // NOTE: We need copy to have a redirection character at
                        // least somewhere
  Sequence sequence_struct;
  sequence_struct.redirection_length = 0;
  sequence_struct.component = NULL;
  char *separators = "><|";
  char *pipeline;
  char *rest = sequence;
  Redirection_Type redirection = NONE_REDIRECTION;
  while ((pipeline = strtok_r(rest, separators, &rest)) != NULL) {
    sequence_struct.redirection_length++;
    sequence_struct.component = (SequenceComponent *)realloc(
        sequence_struct.component,
        sequence_struct.redirection_length * sizeof(SequenceComponent));
    if (sequence_struct.component == NULL) {
      slog_error("Failed to allocate memory for pipeline. Closing program...");
      exit(EXIT_FAILURE);
    }
    if (redirection == INPUT_REDIRECTION ||
        redirection == OUTPUT_REDIRECTION) { // NOTE: if previous redirection is
                                             // input, then we
                                             // have only one pipeline
      redirection = get_redirection(
          sequence_copy[rest - sequence - 1]); // NOTE: Can UB happen here?
      sequence_struct.component[sequence_struct.redirection_length - 1] =
          parse_file(pipeline, redirection);
      break;
    }
    redirection = get_redirection(
        sequence_copy[rest - sequence - 1]); // NOTE: Can UB happen here?
    SequenceComponent redirection_struct = {0};
    redirection_struct.type =  PIPELINE_TYPE;
    redirection_struct = parse_redirection(pipeline, redirection);
    sequence_struct.component[sequence_struct.redirection_length - 1] =
        redirection_struct;
  }
  return sequence_struct;
}

/**
 * Parse a line into a Line struct
 * @param line The line to parse
 * @return The parsed Line struct
 */
Line parse_line(char *line) {
  Line line_struct;
  line_struct.sequence_length = 0;
  line_struct.sequence = NULL;
  char *separators = ";";
  char *sequence;
  char *rest = line;
  while ((sequence = strtok_r(rest, separators, &rest)) != NULL) {
    line_struct.sequence_length++;
    line_struct.sequence = (Sequence *)realloc(
        line_struct.sequence, line_struct.sequence_length * sizeof(Sequence));
    if (line_struct.sequence == NULL) {
      slog_error("Failed to allocate memory for sequence. Closing program...");
      exit(EXIT_FAILURE);
    }
    line_struct.sequence[line_struct.sequence_length - 1] =
        parse_sequence(sequence);
  }
  return line_struct;
}

Parser parse(char *input) {
  Parser parser;
  parser.lines_length = 0;
  parser.lines = NULL;
  char *separators = "\n";
  char *line;
  char *rest = input;
  while ((line = strtok_r(rest, separators, &rest)) != NULL) {
    parser.lines_length++;
    parser.lines =
        (Line *)realloc(parser.lines, parser.lines_length * sizeof(Line));
    if (parser.lines == NULL) {
      slog_error("Failed to allocate memory for lines. Closing program...");
      exit(EXIT_FAILURE);
    }
    line = remove_comments(line);
    parser.lines[parser.lines_length - 1] = parse_line(line);
  }

  return parser;
}
