#include "../parse/parse.h"
#include "executor.h"
#include <fcntl.h>
#include <slog.h>
#include <sys/wait.h>
#include <unistd.h>

Exec_Result exec_command(ExecUnit unit) {
  // We will execute the command with the input and output file descriptors
  // set. We will use the execvp function to execute the command.
  int pid = fork();
  if (pid == -1) {
    return (Exec_Result){
        .error_message = "Failed to fork",
        .result = EXEC_FAILURE,
    };
  } else if (pid == 0) {
    dup2(unit.input_fd, 0);
    dup2(unit.output_fd, 1);
    execvp(unit.command.executable, unit.command.args);
    close(unit.input_fd);
    close(unit.output_fd);
    return (Exec_Result){
        .error_message = "\0",
        .result = EXEC_SUCCESS,
    };
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish

    if (status != 0) {
      // Child process had an error
      return (Exec_Result){
          .error_message = "Command execution failed",
          .result = EXEC_FAILURE,
      };
    }

    return (Exec_Result){
        .error_message = "\0",
        .result = EXEC_SUCCESS,
    };
  }
}

// How executor should work in general?
// If we have redirection with some redirection, we should look at the
// redirection. If the redirection is input or output, we should go to the next
// redirection
Exec_Result exec_sequence(Sequence *sequence) {
  for (size_t k = 0; k < sequence->redirection_length; ++k) {
    SequenceComponent s_component = sequence->component[k];
    // Create pipe file
    char *pipe_name =
        "/tmp/pipe"; // TODO: change for compatibility with windows
    int pipe_fd = open(pipe_name, O_RDWR);
    switch (s_component.type) { // If the redirection is a file, we will open
                                // the file and redirect the input or output
                                // accordingly.
    case FILE_TYPE:
      if (s_component.component.file.redirection == INPUT_REDIRECTION) {
        int fd = open(s_component.component.file.file, O_RDONLY);
        dup2(fd, 0);
        close(fd);
      } else if (s_component.component.file.redirection == OUTPUT_REDIRECTION) {
        int fd =
            open(s_component.component.file.file, O_WRONLY | O_CREAT, 0644);
        dup2(fd, 1);
        close(fd);
      } else {
        slog_error("Invalid redirection type");
      }
      break;
    case PIPELINE_TYPE: // It's either a pipeline, input redirectio or output
                        // redirection
      // 1. command is a input file, outout file and the command, by default it
      // is a command + stdin + stdout
      ExecUnit unit = {
          .command = s_component.component.pipeline.command,
          .input_fd = DEFAULT_INPUT,
          .output_fd = DEFAULT_OUTPUT,
      };
      // Check next sequence component for pipeline
      if (s_component.component.pipeline.redirection == PIPE_REDIRECTION) {
        // Create a pipe and set the input to the pipe
        unit.output_fd = pipe_fd;
        return exec_command(unit);
      }
      // Check whether there is an input redirection in the current pipeline and
      // in the next sequence component
      if (s_component.component.pipeline.redirection == INPUT_REDIRECTION) {
        // Open the file that is in the next sequence component
        if (k + 1 >= sequence->redirection_length) {
          slog_error("Invalid redirection. It was parsed as INPUT_REDIRECTION "
                     "but there is no file to redirect to");
        }
        unit.input_fd =
            open(sequence->component[k + 1].component.file.file, O_RDONLY);
      }
      // Check next sequence component for output redirection
      if (s_component.component.pipeline.redirection == OUTPUT_REDIRECTION) {
        // Open the file that is in the next sequence component
        if (k + 1 >= sequence->redirection_length) {
          slog_error("Invalid redirection. It was parsed as OUTPUT_REDIRECTION "
                     "but there is no file to redirect to");
        }
        unit.output_fd = open(sequence->component[k + 1].component.file.file,
                              O_WRONLY | O_CREAT, 0644);
      }
      return exec_command(unit);
      break;
    };
  }
}

void exec(Parser parser) {
  // We will iterativetly execute the lines in the parse, and sequence in each
  // line. In each sequence we will execute pipelines iteratively and handle
  // the redirections.
  for (size_t i = 0; i < parser.lines_length; ++i) {
    Line line = parser.lines[i];
    for (size_t j = 0; j < line.sequence_length; ++j) {
      Sequence sequence = line.sequence[j];
      int pipefd[2];
      int input = 0;
      for (size_t k = 0; k < sequence.redirection_length; ++k) {
        SequenceComponent redirection = sequence.component[k];
        switch (redirection.type) {
        case FILE_TYPE:
          if (redirection.component.file.redirection == INPUT_REDIRECTION) {
            int fd = open(redirection.component.file.file, O_RDONLY);
            dup2(fd, 0);
            close(fd);
          } else if (redirection.component.file.redirection ==
                     OUTPUT_REDIRECTION) {
            int fd =
                open(redirection.component.file.file, O_WRONLY | O_CREAT, 0644);
            dup2(fd, 1);
            close(fd);
          }
          break;
        case PIPELINE_TYPE:
          pipe(pipefd);
          if (fork() == 0) {
            dup2(input, 0);
            if (k < sequence.redirection_length - 1) {
              dup2(pipefd[1], 1);
            }
            close(pipefd[0]);
            execvp(redirection.pipeline_file.pipeline.command.executable,
                   redirection.pipeline_file.pipeline.command.args);
          }
          close(pipefd[1]);
          input = pipefd[0];
          break;
        }
      }
    }
  }
}
