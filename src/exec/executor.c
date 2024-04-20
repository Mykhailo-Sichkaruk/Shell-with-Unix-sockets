#include "executor.h"

// Function to create or open the file to be used as a "pipe"
int open_pipe_file() {
  const char *pipe_name =
      "/tmp/pipe"; // TODO: change for compatibility with windows
  int pipe_fd = open(pipe_name, O_RDWR | O_CREAT, 0644);
  if (pipe_fd == -1) {
    slog_error("Failed to open or create pipe file");
    exit(EXIT_FAILURE);
  }
  slog_debug("Pipe file opened successfully %d", pipe_fd);
  return pipe_fd;
}

Exec_Result exec_command(ExecUnit unit) {
  // We will execute the command with the input and output file descriptors
  // set. We will use the execvp function to execute the command.
  int pid = fork();
  if (pid == -1) {
    slog_error("Failed to fork");
    return (Exec_Result){
        .error_message = "Failed to fork",
        .result = EXEC_FAILURE,
    };
  } else if (pid == 0) {
    char **exec_args = malloc((unit.command.args_length + 2) * sizeof(char *));
    if (exec_args == NULL) {
      perror("Failed to allocate memory for exec_args");
      exit(EXIT_FAILURE); // In a child process, we should exit on failure.
      return (Exec_Result){
          .error_message = "Failed to allocate memory for exec_args",
          .result = EXEC_FAILURE,
      };
    }
    // Set up arguments array: first argument is the executable
    exec_args[0] = unit.command.executable;
    for (size_t i = 0; i < unit.command.args_length; ++i) {
      exec_args[i + 1] = unit.command.args[i];
    }
    exec_args[unit.command.args_length + 1] = NULL;

    dup2(unit.input_fd, 0);
    dup2(unit.output_fd, 1);
    execvp(unit.command.executable, exec_args);
    close(unit.input_fd);
    close(unit.output_fd);
    free(exec_args);
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
Exec_Result exec_sequence(Sequence *sequence, int output_fd) {
  ExecUnit unit = {
      .command = sequence->component[0].component.pipeline.command,
      .input_fd = DEFAULT_INPUT,
      .output_fd = output_fd,
  };
  // Create pipe file
  int pipe_fd = open_pipe_file();
  for (size_t k = 0; k < sequence->redirection_length; ++k) {
    SequenceComponent s_component = sequence->component[k];
    print_component(s_component);
    unit.command = s_component.component.pipeline.command;
    switch (s_component.type) { // If the redirection is a file, we will open
                                // the file and redirect the input or output
                                // accordingly.
    case FILE_TYPE:
      slog_debug("Execute component: file");
      if (s_component.component.file.redirection == INPUT_REDIRECTION) {
        int fd = open(s_component.component.file.file, O_RDONLY);
        dup2(fd, 0);
        close(fd);
      } else if (s_component.component.file.redirection == OUTPUT_REDIRECTION) {
        int fd =
            open(s_component.component.file.file, O_WRONLY | O_CREAT, 0644);
        dup2(fd, 1);
        close(fd);
      } else if (s_component.component.file.redirection == NONE_REDIRECTION) {
      } else if (s_component.component.file.redirection == PIPE_REDIRECTION) {
        unit.output_fd = pipe_fd;
        print_execution_unit(unit);
        Exec_Result result = exec_command(unit); //????
        if (result.result == EXEC_FAILURE) {
          slog_error("Failed to execute command");
          return result;
        } else {
          slog_debug("Executmi pipeline");
          unit.input_fd = pipe_fd;
          unit.output_fd = output_fd;
        }
      } else {
        slog_error("Invalid redirection type in file component");
        exit(EXIT_FAILURE);
      }
      break;
    case PIPELINE_TYPE: // It's either a pipeline, input redirectio or output
                        // redirection
      // 1. command is a input file, outout file and the command, by default it
      // is a command + stdin + stdout
      // Check next sequence component for pipeline
      if (s_component.component.pipeline.redirection == PIPE_REDIRECTION) {
        slog_debug("Execute component: pipeline_redirection");
        print_execution_unit(unit);
        // Create a pipe and set the input to the pipe
        slog_debug("Set current command output to pipe");
        unit.output_fd = pipe_fd;
        // exec_command and then exec next command with input from pipe
        Exec_Result result = exec_command(unit); //????
        if (result.result == EXEC_FAILURE) {
          slog_error("Failed to execute command");
          return result;
        } else {
          slog_debug("Execute next command with input from pipe");
          // Set the input of the next command to the pipe
          unit.input_fd = pipe_fd;
          unit.output_fd = output_fd;
        }
      } else if (s_component.component.pipeline.redirection ==
                 INPUT_REDIRECTION) {
        slog_debug("Execute component: input_redirection");
        print_execution_unit(unit);
        if (k + 1 >= sequence->redirection_length) {
          slog_error("Invalid redirection. It was parsed as INPUT_REDIRECTION "
                     "but there is no file to redirect to");
          return (Exec_Result){
              .error_message = "Invalid redirection",
              .result = EXEC_FAILURE,
          };
        }
        unit.input_fd =
            open(sequence->component[k + 1].component.file.file, O_RDONLY);
        if (unit.input_fd == -1) {
          slog_error("Failed to open input file: %s", strerror(errno));
          return (Exec_Result){
              .error_message = "Failed to open input file",
              .result = EXEC_FAILURE,
          };
        }
        slog_debug("Input file opened successfully: %d", unit.input_fd);
        Exec_Result result = exec_command(unit);
        if (result.result == EXEC_FAILURE) {
          slog_error("Failed to execute command");
          return result;
        } else {
          slog_debug("Command executed successfully");
        }
      } else if (s_component.component.pipeline.redirection ==
                 OUTPUT_REDIRECTION) {
        // Open the file that is in the next sequence component
        slog_debug("Execute component: output_redirection");
        print_execution_unit(unit);
        if (k + 1 >= sequence->redirection_length) {
          slog_error("Invalid redirection. It was parsed as OUTPUT_REDIRECTION "
                     "but there is no file to redirect to");
        }
        unit.output_fd = open(sequence->component[k + 1].component.file.file,
                              O_WRONLY | O_CREAT, 0644);
        if (unit.input_fd == -1) {
          slog_error("Failed to open input file: %s", strerror(errno));
          return (Exec_Result){
              .error_message = "Failed to open input file",
              .result = EXEC_FAILURE,
          };
        }
        slog_debug("Input file opened successfully: %d", unit.input_fd);
        Exec_Result result = exec_command(unit);
        if (result.result == EXEC_FAILURE) {
          slog_error("Failed to execute command");
          return result;
        } else {
          slog_debug("Command executed successfully");
        }
      } else if (s_component.component.pipeline.redirection ==
                 NONE_REDIRECTION) {
        unit.output_fd = output_fd;
        print_execution_unit(unit);
        Exec_Result result = exec_command(unit); //????
        if (result.result == EXEC_FAILURE) {
          slog_error("Failed to execute command");
          return result;
        } else {
          slog_debug("Executmi pipeline");
          unit.input_fd = pipe_fd;
          unit.output_fd = output_fd;
        }
      } else {
        print_execution_unit(unit);
        slog_error("Invalid redirection type in pipeline component: %d",
                   s_component.component.pipeline.redirection);
        exit(EXIT_FAILURE);
      }
      break;
    default:
      slog_error("Unexpected sequence component type.");
      exit(EXIT_FAILURE);
      break;
    }
  }

  return (Exec_Result){
      .error_message = "\0",
      .result = EXEC_SUCCESS,
  };
}

Exec_Result exec(Parser parser, int output_fd) {
  // We will iterativetly execute the lines in the parse, and sequence in each
  // line. In each sequence we will execute pipelines iteratively and handle
  // the redirections.
  for (size_t i = 0; i < parser.lines_length; ++i) {
    slog_debug("Executing line %zu", i);
    Line line = parser.lines[i];
    for (size_t j = 0; j < line.sequence_length; ++j) {
      slog_debug("Executing sequence %zu. `%s`", j, line.sequence[j].str);
      Sequence sequence = line.sequence[j];
      Exec_Result result = exec_sequence(&sequence, output_fd);
      if (result.result == EXEC_FAILURE) {
        slog_error("Failed to execute sequence");
        return result;
      }
    }
  }

  return (Exec_Result){
      .error_message = "\0",
      .result = EXEC_SUCCESS,
  };
}
