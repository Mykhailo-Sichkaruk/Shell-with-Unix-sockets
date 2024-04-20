#include "../src/log.c"
#include <parse.h>
#include <executor.h>
#include <assert.h>
#include <unistd.h>

void parse_sequence_and_exec() {
  // char command[] = "echo Hello, world! | wc -c | wc -l";
  char command[] = "echo asdfsaf > test2.txt";
  Sequence parsed = parse_sequence(command);
  print_sequence(parsed);
  Exec_Result result = exec_sequence(&parsed);
  free_sequence(&parsed);
  assert(result.result == EXEC_SUCCESS);
  slog_info("Executed command successfully");
}

void parse_and_exec() {
  char command[] = "echo Hello, world! | wc ; date > date.txt; echo Hello000000000, world!"; // TODO: Trim line
  Parser parser = parse(command);
  exec(parser);
  free_parser(&parser);
}

int main() {
  init_logger("test_log");
  parse_sequence_and_exec();
  parse_and_exec();
  return 0;
}
