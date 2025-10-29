cmake_minimum_required(VERSION 3.12)
project(TinySearch CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tiny_search
    src/main.cpp
    src/Tokenizer.cpp
    src/InvertedIndex.cpp
    src/DocumentStore.cpp
    src/Ranker.cpp
    src/PageRank.cpp
    src/QueryEngine.cpp
)

if (MSVC)
    target_compile_options(tiny_search PRIVATE /W4)
else()
    target_compile_options(tiny_search PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()

target_include_directories(tiny_search PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
