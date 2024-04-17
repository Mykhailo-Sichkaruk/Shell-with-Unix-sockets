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

# executor tests
add_executable(executor_tests 
./test/executor.c
./src/shell/executor.c
)
target_compile_options(executor_tests PRIVATE ${DEBUG_OPTIONS})
target_link_libraries(executor_tests PRIVATE slog pthread)
add_custom_target(executor_test
    COMMAND ./executor_tests
    DEPENDS executor_tests
    COMMENT "Running the executor tests..."
)


# executor + parser tests
add_executable(exec_parse_tests 
# ./test/exec_parse.c
./src/shell/executor.c
./src/parse/parse.c
)
target_compile_options(exec_parse_tests PRIVATE ${DEBUG_OPTIONS})
target_link_libraries(exec_parse_tests PRIVATE slog pthread)
add_custom_target(exec_parse_test
    COMMAND ./exec_parse_tests
    DEPENDS exec_parse_tests
    COMMENT "Running the executor and parse tests..."
)

# Add a library target for the parser
add_library(parse_lib SHARED
    ./src/parse/parse.c
)

target_include_directories(parse_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Set properties for the library
set_target_properties(parse_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/parse.h
)

add_executable(chell 
./src/main.c
./src/shell/executor.c
./src/shell/utils.c
)
target_compile_options(chell PRIVATE ${RELEASE_OPTIONS})
target_link_libraries(chell PRIVATE slog pthread parse_lib)  # Link the parse library here
add_custom_target(release
    COMMAND ./chell
    DEPENDS chell
    COMMENT "Running the ShellProject in release mode..."
)

install(TARGETS parse_lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

add_executable(parser_tests ./src/parse/test/parse.c)
target_compile_options(parser_tests PRIVATE ${RELEASE_OPTIONS})
target_link_libraries(parser_tests PRIVATE slog pthread parse_lib)  # Link the parse library here
add_custom_target(parser_test
    COMMAND ./parser_tests
    DEPENDS parser_tests
    COMMENT "Running the ShellProject in release mode..."
)
