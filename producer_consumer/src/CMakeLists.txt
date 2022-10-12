include_directories(${ProducerConsumer_SOURCE_DIR}/producer_consumer/include)
include_directories(${ProducerConsumer_SOURCE_DIR}/producer_consumer/include/valgrind)
FILE(GLOB CPPSources *.cpp)
add_library(ProducerConsumer ${CPPSources})