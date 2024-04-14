#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stddef.h> // For size_t

typedef enum {
  NONE_REDIRECTION = 0,
  INPUT_REDIRECTION = '<',
  OUTPUT_REDIRECTION = '>',
  PIPE_REDIRECTION = '|',
} Redirection_Type;

typedef enum {
  PIPELINE_TYPE,
  FILE_TYPE,
} PIPELINE_FILE_TYPE;

typedef struct Command { // An executable command with arguments
  char *executable;
  char **args;
  size_t args_length;
} Command;

typedef struct Pipeline { // A command and a redirection
  Command command;
  Redirection_Type redirection;
} Pipeline;

typedef struct FileRedirection {
  char *file;
  Redirection_Type redirection;
} FileRedirection;

typedef union Pipeline_File {
  FileRedirection file;
  Pipeline pipeline;
} Pipeline_File;

typedef struct SequenceComponentStruct {
  PIPELINE_FILE_TYPE type;
  Pipeline_File component;
} SequenceComponent;

typedef struct Sequence { // An array of terms
  SequenceComponent *component;
  size_t redirection_length;
} Sequence;

typedef struct Line { // An array of sequences
  Sequence *sequence;
  size_t sequence_length;
} Line;

typedef struct Parser {
  Line *lines;
  size_t lines_length;
} Parser;

// Function declarations
char *remove_comments(char *str);
Redirection_Type get_redirection(char redirection);
char *trim(char *str);
SequenceComponent parse_file(char *file, Redirection_Type redirection);
SequenceComponent parse_redirection(char *pipeline, Redirection_Type redirection);
Sequence parse_sequence(char *sequence);
Line parse_line(char *line);
Parser parse(char *input);

#endif // PARSE_H
