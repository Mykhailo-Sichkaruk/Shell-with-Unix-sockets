#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

#include "parse.h"

void print_file(FileRedirection fileRedirection);
void print_command(Command command);
void print_component(SequenceComponent redirection);
void print_sequence(Sequence sequence);
void print_line(Line line);

void free_command(Command *cmd);
void free_sequence_component(SequenceComponent *component);
void free_sequence(Sequence *sequence);
void free_line(Line *line);
void free_parser(Parser *parser);
#endif // PARSE_UTILS_H
