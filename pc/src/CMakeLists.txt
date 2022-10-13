include_directories(${ProducerConsumer_SOURCE_DIR}/pc/include)
include_directories(${ProducerConsumer_SOURCE_DIR}/pc/include/valgrind)
FILE(GLOB CPPSources *.cpp)
add_library(ProducerConsumer ${CPPSources})