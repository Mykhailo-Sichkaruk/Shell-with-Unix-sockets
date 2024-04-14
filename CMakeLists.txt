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

# executor tests
add_executable(executor_tests 
./test/executor.c
./test/parse.c
./src/parse/parse.c
./src/parse/parse.h
./src/log.c
)
target_compile_options(executor_tests PRIVATE ${DEBUG_OPTIONS})
target_link_libraries(executor_tests PRIVATE slog pthread)
add_custom_target(executor_test
    COMMAND ./executor_tests
    DEPENDS executor_tests
    COMMENT "Running the executor tests..."
)
