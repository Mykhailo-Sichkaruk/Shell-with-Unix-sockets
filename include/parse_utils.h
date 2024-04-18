#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

#include "parse.h"

void print_file(FileRedirection fileRedirection);
void print_command(Command command);
void print_component(SequenceComponent redirection);
void print_sequence(Sequence sequence);
void print_line(Line line);

#endif // PARSE_UTILS_H
