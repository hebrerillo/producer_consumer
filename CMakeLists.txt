cmake_minimum_required(VERSION 3.13.4)
project(ProducerConsumer)

add_compile_options(-g -Wall -Wextra -pedantic -Werror)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProducerConsumer_SOURCE_DIR}/bin)

add_subdirectory(producer_consumer)
add_subdirectory(testApps)

