include_directories(${ProducerConsumer_SOURCE_DIR}/testApps/include)
# I assume you want to use LibProject as a library in MainProject.
include_directories(${ProducerConsumer_SOURCE_DIR}/pc/include)
link_directories(${ProducerConsumer_SOURCE_DIR}/pc/src)

add_executable(pcshell main.cpp)
target_link_libraries(pcshell ProducerConsumer pthread)

add_executable(pctest test.cpp)
target_link_libraries(pctest ProducerConsumer pthread ${ProducerConsumer_SOURCE_DIR}/lib/libgtest.a)