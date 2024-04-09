#include "../src/log.c"
#include "../src/parse/parse.h"
#include <assert.h>
#include "utils.c"

void test_parse_sequence() {
  char sequence[] = "cmd1 arg1 > output.txt";

  Sequence result = parse_sequence(sequence);
  print_sequence(result); 
  assert(result.redirection_length == 2); // Because there are 2 redirections: `cmdl argl` + `>` and `output.txt` + `none`
  assert(result.redirection[0].type == PIPELINE_TYPE);
  assert(strcmp(result.redirection[1].pipeline_file.file.file, "output.txt") == 0);

  printf("test_parse_sequence passed.\n");
}

void test_parse_line() {
  char line[] = "cmd1 arg1; cmd2 arg2 > output.txt";

  Line result = parse_line(line);
  print_line(result);
  assert(result.sequence_length == 2);
  assert(result.sequence[0].redirection_length == 1);
  assert(result.sequence[1].redirection_length == 2);
  assert(strcmp(result.sequence[1].redirection[1].pipeline_file.file.file,
                "output.txt") == 0);

  printf("test_parse_line passed.\n");
}
 
void test_parse() {
    char input[] = "cmd1 arg1\n cmd2 arg2 > output.txt";

    Parser result = parse(input);

    assert(result.lines_length == 2);
    assert(result.lines[0].sequence_length == 1);
    assert(result.lines[1].sequence_length == 1);
    assert(result.lines[1].sequence[0].redirection_length == 2);
    assert(strcmp(result.lines[1].sequence[0].redirection[1].pipeline_file.file.file, "output.txt") == 0);

    printf("test_parse passed.\n");
}

int main() {
  init_logger("test_log");
  test_parse_sequence();
  test_parse_line();
  test_parse();
  return 0;
}
