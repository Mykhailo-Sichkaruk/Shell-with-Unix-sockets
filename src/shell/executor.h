#include "../parse/parse.h"

typedef enum {
    EXEC_SUCCESS,
    EXEC_FAILURE,
} EXEC_RESULT_TYPE;

typedef struct {
  char *error_message;
  EXEC_RESULT_TYPE result;
} Exec_Result;

typedef struct ExecUnit {
  Command command;
  int input_fd;
  int output_fd;
} ExecUnit;

// Declare STDOUT as DEFAULT_OUTPUT
#define DEFAULT_OUTPUT 1
#define DEFAULT_INPUT 0
