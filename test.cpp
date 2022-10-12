#include <chrono>
#include <math.h>
#include "test.h"
#include "valgrind/include/memcheck.h"

void ProducerConsumerTest::SetUp()
{
    valgrindCheck_.leakCheckInit();
}

void ProducerConsumerTest::TearDown()
{
    for(auto itemBuffer: buffer_)
    {
        delete itemBuffer;
    }

    buffer_.clear();

    valgrindCheck_.leakCheckEnd();
}

void ProducerConsumerTest::addElementsToBuffer(size_t size, size_t numberOfFilledElements)
{
    for(size_t i = 0; i < size; ++i)
    {
        buffer_.push_back(new BufferItem(i < numberOfFilledElements));
    }
}

void ProducerConsumerTest::createProducersAndConsumers(const PC_Params& params)
{
    size_t indexProducers = 0;
    size_t indexConsumers = 0;

    while(indexProducers < params.numberOfProducers || indexConsumers < params.numberOfConsumers)
    {
        if (indexProducers < params.numberOfProducers)
        {
            params.manager->addProducer(std::chrono::milliseconds(params.producersDelay));
            indexProducers++;
        }

        if (indexConsumers < params.numberOfConsumers)
        {
            params.manager->addConsumer(std::chrono::milliseconds(params.consumersDelay));
            indexConsumers++;
        }
    }
}

TEST_F(ProducerConsumerTest, AfterInsertingALotOfConsumersAndProducersWithLongDelayIntoABigBuffer_ThenTheQuitProcessIsQuick)
{
    const size_t NUMBER_CONSUMERS = 90;
    const size_t NUMBER_PRODUCERS = 180;
    const uint64_t DELAY = 500;
    const size_t BIG_BUFFER_SIZE = 2000;
    uint64_t MAX_ELAPSED_TIME = 40; //The maximum elapsed time before and after stopping the manager, in milliseconds.
#ifdef LINUX
    if (RUNNING_ON_VALGRIND)
    {
        MAX_ELAPSED_TIME = 1500;
    }
#endif


    addElementsToBuffer(BIG_BUFFER_SIZE);
    ProducerConsumerManager manager(buffer_);
    
    PC_Params params(NUMBER_PRODUCERS, NUMBER_CONSUMERS, DELAY, DELAY, &manager);
    createProducersAndConsumers(params);

    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY * 2));
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    manager.stop();

    std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - begin);

    EXPECT_LT(elapsedTime.count(), MAX_ELAPSED_TIME);
}

TEST_F(ProducerConsumerTest, WhenAddingOnlyOneProducer_ThenAfterWaitingTheSharedBufferIsFull)
{
    const size_t BUFFER_SIZE = 100;
    const uint64_t DELAY = 5;
    uint64_t EXTRA_DELAY = 1;

    addElementsToBuffer(BUFFER_SIZE);
    ProducerConsumerManager manager(buffer_);
    manager.addProducer(std::chrono::milliseconds(DELAY));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY * (BUFFER_SIZE + EXTRA_DELAY)));
    manager.stop();
    
    for(auto bufferItem: buffer_)
    {
        EXPECT_TRUE((*bufferItem));
    }
}

TEST_F(ProducerConsumerTest, WhenAddingOnlyOneConsumerInAFullSharedBuffer_ThenAfterWaitingTheSharedBufferIsEmpty)
{
    const size_t BUFFER_SIZE = 100;
    const uint64_t DELAY = 5;
    uint64_t EXTRA_DELAY = 3;

#ifdef LINUX
    if (RUNNING_ON_VALGRIND)
    {
        EXTRA_DELAY = 14;
    }
#endif

    addElementsToBuffer(BUFFER_SIZE, BUFFER_SIZE);
    ProducerConsumerManager manager(buffer_);
    manager.addConsumer(std::chrono::milliseconds(DELAY));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY * (BUFFER_SIZE + EXTRA_DELAY)));
    manager.stop();
    
    for(auto bufferItem: buffer_)
    {
        EXPECT_FALSE((*bufferItem));
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}