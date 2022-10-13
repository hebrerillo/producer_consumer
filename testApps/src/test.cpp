#include <chrono>
#include <math.h>
#include "test.h"
#include "valgrind/memcheck.h"

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

    if (RUNNING_ON_VALGRIND)
    {
        MAX_ELAPSED_TIME = 1500;
    }

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
    uint64_t EXTRA_DELAY = 15;

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

    if (RUNNING_ON_VALGRIND)
    {
        EXTRA_DELAY = 14;
    }

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

TEST_F(ProducerConsumerTest, WhenAddingAVeryFastProducerAndSomeSlowConsumersInAnEmptySharedBuffer_ThenAfterWaitingTheSharedBufferIsFull)
{
    const size_t BUFFER_SIZE = 10;
    const uint64_t DELAY_PRODUCER = 40;
    const size_t NUMBER_PRODUCERS = 1;
    const uint64_t DELAY_CONSUMERS = 400;
    const size_t NUMBER_CONSUMERS = 5;
    
    addElementsToBuffer(BUFFER_SIZE);
    ProducerConsumerManager manager(buffer_);
    
    PC_Params params(NUMBER_PRODUCERS, NUMBER_CONSUMERS, DELAY_PRODUCER, DELAY_CONSUMERS, &manager);
    createProducersAndConsumers(params);

    size_t i = 0;
    while(manager.getCurrentIndex() != BUFFER_SIZE && i < (DELAY_PRODUCER * 2))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_PRODUCER/2));
        i++;
    }
    manager.stop();
    
    EXPECT_EQ(manager.getCurrentIndex(), BUFFER_SIZE);
    for(auto bufferItem: buffer_)
    {
        EXPECT_TRUE((*bufferItem));
    }
}

TEST_F(ProducerConsumerTest, WhenAddingSeveralProducersThatFillABuffer_ThenAfterRemovingAllProducersAndAddinConsumersTheBufferIsEmpty)
{
    const size_t BUFFER_SIZE = 10;
    const uint64_t DELAY = 20;
    const size_t NUMBER_PRODUCERS = 5;
    const size_t NUMBER_CONSUMERS = 5;

    addElementsToBuffer(BUFFER_SIZE);
    ProducerConsumerManager manager(buffer_);

    //Adding only producers
    PC_Params params(NUMBER_PRODUCERS, 0, DELAY, 0, &manager);
    createProducersAndConsumers(params);
    
    size_t i = 0;
    while(manager.getCurrentIndex() != BUFFER_SIZE && i < (DELAY * 2))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY/2));
        i++;
    }

    //Let's check the producers filled the whole buffer
    EXPECT_EQ(manager.getCurrentIndex(), BUFFER_SIZE);
    for(auto bufferItem: buffer_)
    {
        EXPECT_TRUE((*bufferItem));
    }

    //Now remove all producers and add some consumers
    manager.removeProducers();
    params = PC_Params(0, NUMBER_CONSUMERS, 0, DELAY, &manager);
    createProducersAndConsumers(params);

    i = 0;
    while(manager.getCurrentIndex() != 0 && i < (DELAY * 2))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY/2));
        i++;
    }

    //Let's check the consumers emptied the whole buffer
    EXPECT_EQ(manager.getCurrentIndex(), 0);
    for(auto bufferItem: buffer_)
    {
        EXPECT_FALSE((*bufferItem));
    }
    manager.stop();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
