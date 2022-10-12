cmake_minimum_required(VERSION 3.13.4)
project(ProducerConsumer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProducerConsumer_SOURCE_DIR}/bin)

add_subdirectory(producer_consumer)
add_subdirectory(testApps)

