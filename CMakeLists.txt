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

# Source files
set(EXECUTOR_SRC
  ./src/exec/executor.c
  ./src/exec/executor_utils.c
)
# Debug library with sanitizers
add_library(executor_lib_debug SHARED ${EXECUTOR_SRC})
target_compile_options(executor_lib_debug PRIVATE ${DEBUG_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(executor_lib_debug PRIVATE ${SANITIZER_OPTIONS})
# Release library without sanitizers
add_library(executor_lib_release SHARED ${EXECUTOR_SRC})
target_compile_options(parse_lib_release PRIVATE ${RELEASE_OPTIONS})

# Main executable
add_executable(chell ./src/main.c)
target_compile_options(chell PRIVATE ${RELEASE_OPTIONS})
target_link_libraries(chell PRIVATE slog pthread parse_lib_release executor_lib_release)
# Debug executable
add_executable(chell_debug ./src/main.c)
target_compile_options(chell_debug PRIVATE ${DEBUG_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(chell_debug PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(chell_debug PRIVATE slog pthread parse_lib_debug executor_lib_debug)

# Parser tests
add_executable(parser_tests 
  ./test/parse.c
)
target_compile_options(parser_tests PRIVATE ${RELEASE_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(parser_tests PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(parser_tests PRIVATE slog pthread parse_lib_debug)
add_custom_target(parser_test
    COMMAND ./parser_tests
    DEPENDS parser_tests
    COMMENT "Running Parser Tests..."
)

# Executor tests
add_executable(executor_tests 
  ./test/executor.c
)
target_compile_options(executor_tests PRIVATE ${RELEASE_OPTIONS} ${SANITIZER_OPTIONS})
target_link_options(executor_tests PRIVATE ${SANITIZER_OPTIONS})
target_link_libraries(executor_tests PRIVATE slog pthread parse_lib_debug executor_lib_debug)
add_custom_target(executor_test
    COMMAND ./executor_tests
    DEPENDS executor_tests
    COMMENT "Running Executor Tests..."
)
