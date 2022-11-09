cmake_minimum_required(VERSION 3.13.4)
project(ProducerConsumer)

add_compile_options(-g -std=c++17 -Wall -Wextra -pedantic -Werror -DLINUX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProducerConsumer_SOURCE_DIR}/build/bin)

add_subdirectory(pc)
add_subdirectory(testApps)

