cmake_minimum_required(VERSION 3.20)
project(llvm-cpp-compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LLVM SETUP
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs 
    core 
    analysis 
    executionengine 
    mcjit 
    native
    codegen
)


# COMPILER LOGIC LIBRARY
add_library(compiler_lib
    src/frontend/Lexer.cpp
    src/frontend/Parser.cpp
    src/frontend/AST.cpp
    src/codegen/IR_Generator.cpp
    src/backend/JITengine.cpp
)

target_include_directories(compiler_lib PUBLIC include)
target_link_libraries(compiler_lib PUBLIC ${llvm_libs})


# MAIN EXEC
add_executable(compiler src/main.cpp)
target_link_libraries(compiler PRIVATE compiler_lib)

# TESTS (Catch2)
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests 
  tests/test_lexer.cpp
  tests/test_parser.cpp
)
target_link_libraries(unit_tests PRIVATE compiler_lib Catch2::Catch2WithMain)

enable_testing()
add_test(NAME LexerTests COMMAND unit_tests)