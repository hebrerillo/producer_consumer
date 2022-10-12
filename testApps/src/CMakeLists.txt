include_directories(${ProducerConsumer_SOURCE_DIR}/testApps/include)
# I assume you want to use LibProject as a library in MainProject.
include_directories(${ProducerConsumer_SOURCE_DIR}/producer_consumer/include)
link_directories(${ProducerConsumer_SOURCE_DIR}/producer_consumer/src)
add_executable(MainProject main.cpp)
target_link_libraries(MainProject ProducerConsumer pthread)