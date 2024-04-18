cmake_minimum_required(VERSION 3.14)
project(ShellProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

set(DEBUG_OPTIONS
  -Wall
  -Wextra
  -Werror
  -pedantic-errors
  -O0
  -g
  -DDEBUG
)

set(RELEASE_OPTIONS
  -Wall
  -Wextra
  -Werror
  -pedantic-errors
  -O3
)

set(SANITIZER_OPTIONS
  -fsanitize=address
  -fsanitize=undefined
  -fno-omit-frame-pointer
  -g
)

# Executor tests
add_executable(executor_tests 
  ./test/executor.c
  ./src/shell/executor.c
)
target_compile_options(executor_tests PRIVATE ${DEBUG_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(executor_tests PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(executor_tests PRIVATE slog pthread)
add_custom_target(executor_test
    COMMAND ./executor_tests
    DEPENDS executor_tests
    COMMENT "Running the executor tests..."
)

# Executor + parser tests
add_executable(exec_parse_tests 
  ./src/shell/executor.c
  ./src/parse/parse.c
)
target_compile_options(exec_parse_tests PRIVATE ${DEBUG_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(exec_parse_tests PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(exec_parse_tests PRIVATE slog pthread)
add_custom_target(exec_parse_test
    COMMAND ./exec_parse_tests
    DEPENDS exec_parse_tests
    COMMENT "Running the executor and parse tests..."
)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(PARSE_SRC
  ./src/parse/parse.c
  ./src/parse/parse_utils.c
)
# Debug library with sanitizers
add_library(parse_lib_debug SHARED ${PARSE_SRC})
target_compile_options(parse_lib_debug PRIVATE ${DEBUG_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(parse_lib_debug PRIVATE ${SANITIZER_OPTIONS})

# Release library without sanitizers
add_library(parse_lib_release SHARED ${PARSE_SRC})
target_compile_options(parse_lib_release PRIVATE ${RELEASE_OPTIONS})

# Main executable
add_executable(chell 
  ./src/main.c
  ./src/shell/executor.c
  ./src/shell/utils.c
)
target_compile_options(chell PRIVATE ${RELEASE_OPTIONS})
target_link_libraries(chell PRIVATE slog pthread parse_lib_release)
add_custom_target(release
    COMMAND ./chell
    DEPENDS chell
    COMMENT "Running the ShellProject in release mode..."
)

# Parser tests
add_executable(parser_tests 
  ./test/parse/parse.c
)
target_compile_options(parser_tests PRIVATE ${RELEASE_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(parser_tests PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(parser_tests PRIVATE slog pthread parse_lib_debug)
add_custom_target(parser_test
    COMMAND ./parser_tests
    DEPENDS parser_tests
    COMMENT "Running Parser Tests..."
)
