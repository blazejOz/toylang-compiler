cmake_minimum_required(VERSION 3.20)
project(llvm-cpp-compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs Core)

add_executable(compiler 
    src/main.cpp
    src/1_lexer/Lexer.cpp
    )

target_link_libraries(compiler ${llvm_libs})
target_include_directories(compiler PUBLIC include)


#TESTS
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests 
    tests/test_lexer.cpp
    src/1_lexer/Lexer.cpp
)

target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(unit_tests PRIVATE include)