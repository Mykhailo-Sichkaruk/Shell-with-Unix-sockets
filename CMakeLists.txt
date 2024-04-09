cmake_minimum_required(VERSION 3.14)
project(ShellProject VERSION 1.0 LANGUAGES C)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_STANDARD 11)
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

add_executable(shell_debug ex.c)
target_compile_options(shell_debug PRIVATE ${DEBUG_OPTIONS})
add_custom_target(debug
    COMMAND ./shell_debug
    DEPENDS shell_debug
    COMMENT "Running the ShellProject in debug mode..."
)

# main.c
add_executable(chell ./src/main.c)
target_compile_options(chell PRIVATE ${RELEASE_OPTIONS})
target_link_libraries(chell PRIVATE slog pthread)
add_custom_target(release
    COMMAND ./chell
    # COMMAND rm -f chell
    DEPENDS chell
    COMMENT "Running the ShellProject in release mode..."
)

# parser tests
add_executable(parser_tests 
./test/parse.c
./src/parse/parse.c
./src/parse/parse.h
)
target_compile_options(parser_tests PRIVATE ${DEBUG_OPTIONS})
target_link_libraries(parser_tests PRIVATE slog pthread)
add_custom_target(parser_test
    COMMAND ./parser_tests
    DEPENDS parser_tests
    COMMENT "Running the parser tests..."
)
