#include "../src/log.c"
#include <assert.h>
#include <parse.h>

void test_parse_sequence_simple() {
  char sequence[] = "echo Hello, world!";
  Sequence result = parse_sequence(sequence);
  print_sequence(result);
  assert(result.redirection_length == 1);
  assert(result.component[0].type == PIPELINE_TYPE);
  assert(strcmp(result.component[0].component.pipeline.command.executable,
                "echo") == 0);
  free_sequence(&result);
  slog("test_parse_sequence_simple passed.");
}

void test_parse_sequence() {
  char sequence[] = "cmd1 arg1 > output.txt";

  Sequence result = parse_sequence(sequence);
  print_sequence(result);
  assert(result.redirection_length ==
         2); // Because there are 2 redirections: `cmdl argl` + `>` and
             // `output.txt` + `none`
  assert(result.component[0].type == PIPELINE_TYPE);
  assert(strcmp(result.component[1].component.file.file, "output.txt") == 0);
  free_sequence(&result);

  slog("test_parse_sequence passed.");
}

void test_parse_line() {
  char line[] = "cmd1 arg1; cmd2 arg2 > output.txt";

  Line result = parse_line(line);
  print_line(result);
  assert(result.sequence_length == 2);
  assert(result.sequence[0].redirection_length == 1);
  assert(result.sequence[1].redirection_length == 2);
  assert(strcmp(result.sequence[1].component[1].component.file.file,
                "output.txt") == 0);

  slog("test_parse_line passed.");
}

void test_parse() {
  char input[] = "cmd1 arg1 cmd2 arg2 > output.txt";

  Parser result = parse(input);

  assert(result.lines_length == 2);
  assert(result.lines[0].sequence_length == 1);
  assert(result.lines[1].sequence_length == 1);
  assert(result.lines[1].sequence[0].redirection_length == 2);
  assert(strcmp(result.lines[1].sequence[0].component[1].component.file.file,
                "output.txt") == 0);

  slog("test_parse passed.");
}

int main() {
  init_logger("test_log");
  test_parse_sequence_simple();
  /* test_parse_sequence(); */
  /* test_parse_line(); */
  /* test_parse(); */
  return 0;
}
