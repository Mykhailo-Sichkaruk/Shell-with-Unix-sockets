#include "../src/log.c"
#include "../src/parse/parse.h"
#include <assert.h>
#include "utils.c"
#include "../src/shell/executor.h"
#include "../src/parse/parse.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Helper function to create a temporary file with a unique name
// Returns the file descriptor of the opened file and the path to the file
int create_temp_file(char* temp_file_path) {
    strcpy(temp_file_path, "/tmp/exec_test_XXXXXX");
    int fd = mkstemp(temp_file_path);
    assert(fd != -1); // Ensure the file was created
    return fd;
}

void test_execute_single_command() {
    char temp_file_path[256];
    int temp_file_fd = create_temp_file(temp_file_path);

    // Prepare the command: echo "Hello, world!" > temp_file
    Command cmd = {
        .executable = "echo",
        .args = (char*[]){"echo", "Hello, world!", NULL},
        .args_length = 2,
    };
    ExecUnit unit = {
        .command = cmd,
        .input_fd = STDIN_FILENO,
        .output_fd = temp_file_fd,
    };

    Exec_Result result = exec_command(unit);
    assert(result.result == EXEC_SUCCESS);

    // Cleanup
    close(temp_file_fd);
    unlink(temp_file_path); // Delete the temporary file

    printf("test_execute_single_command passed.\n");
}

void test_execute_sequence_with_redirection() {
    char temp_file_path[256];
    int temp_file_fd = create_temp_file(temp_file_path);

    // Prepare a simple sequence: echo "Test" > temp_file
    Sequence sequence;
    sequence.redirection_length = 2; // echo + redirection
    sequence.component = malloc(sequence.redirection_length * sizeof(SequenceComponent));

    // Command: echo "Test"
    sequence.component[0].type = PIPELINE_TYPE;
    sequence.component[0].component.pipeline.command = (Command){
        .executable = "echo",
        .args = (char*[]){"echo", "Test", NULL},
        .args_length = 2,
        .redirection = NONE_REDIRECTION,
    };

    // File redirection
    sequence.component[1].type = FILE_TYPE;
    sequence.component[1].component.file = (FileRedirection){
        .file = temp_file_path,
        .redirection = OUTPUT_REDIRECTION,
    };

    Exec_Result result = exec_sequence(&sequence);
    assert(result.result == EXEC_SUCCESS);

    // Cleanup
    free(sequence.component);
    close(temp_file_fd);
    unlink(temp_file_path); // Delete the temporary file

    printf("test_execute_sequence_with_redirection passed.\n");
}

int main() {
    init_logger("test_log");
    test_execute_single_command();
    test_execute_sequence_with_redirection();
    return 0;
}

