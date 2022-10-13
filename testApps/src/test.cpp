#include <chrono>
#include <thread>
#include "test.h"
#include "valgrind/memcheck.h"
#include "bufferItem.h"

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
            IPC::addProducer(std::chrono::milliseconds(params.producersDelay));
            indexProducers++;
        }

        if (indexConsumers < params.numberOfConsumers)
        {
            IPC::addConsumer(std::chrono::milliseconds(params.consumersDelay));
            indexConsumers++;
        }
    }
}

bool ProducerConsumerTest::waitForIndexValue(size_t indexValue, uint64_t delay)
{
    size_t i = 0;
    while(IPC::getCurrentIndex() != indexValue && i < (buffer_.size() * delay * 2))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay/2));
        i++;
    }

    //Here we only check that the maximum number of tries is not reached. Checking the value of the shared buffer 'buffer_' is not safe because
    //a producer or consumer could have modified it right after the loop above.
    return i < (buffer_.size() * delay * 2);
}

TEST_F(ProducerConsumerTest, AfterInsertingALotOfConsumersAndProducersWithLongDelayIntoABigBuffer_ThenTheQuitProcessIsQuick)
{
    const size_t NUMBER_CONSUMERS = 90;
    const size_t NUMBER_PRODUCERS = 180;
    const uint64_t DELAY = 500;
    const size_t BIG_BUFFER_SIZE = 2000;
    uint64_t MAX_ELAPSED_TIME = 40; //The maximum elapsed time before and after stopping the buffer from accepting/returning elements, in milliseconds.

    if (RUNNING_ON_VALGRIND)
    {
        MAX_ELAPSED_TIME = 1500;
    }

    addElementsToBuffer(BIG_BUFFER_SIZE);
    IPC::start(buffer_);

    PC_Params params(NUMBER_PRODUCERS, NUMBER_CONSUMERS, DELAY, DELAY);
    createProducersAndConsumers(params);

    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY * 2));
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    IPC::stop();

    std::chrono::milliseconds elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - begin);

    EXPECT_LT(elapsedTime.count(), MAX_ELAPSED_TIME);
}

TEST_F(ProducerConsumerTest, WhenAddingOnlyOneProducer_ThenAfterWaitingTheSharedBufferIsFull)
{
    const size_t BUFFER_SIZE = 100;
    const uint64_t DELAY = 5;

    addElementsToBuffer(BUFFER_SIZE);
    IPC::start(buffer_);
    IPC::addProducer(std::chrono::milliseconds(DELAY));
    
    EXPECT_TRUE(waitForIndexValue(BUFFER_SIZE, DELAY));
    IPC::stop();
    
    for(auto bufferItem: buffer_)
    {
        EXPECT_TRUE((*bufferItem));
    }
}

TEST_F(ProducerConsumerTest, WhenAddingOnlyOneConsumerInAFullSharedBuffer_ThenAfterWaitingTheSharedBufferIsEmpty)
{
    const size_t BUFFER_SIZE = 100;
    const uint64_t DELAY = 5;

    addElementsToBuffer(BUFFER_SIZE, BUFFER_SIZE);
    IPC::start(buffer_);
    IPC::addConsumer(std::chrono::milliseconds(DELAY));
    
    EXPECT_TRUE(waitForIndexValue(0, DELAY));
    IPC::stop();
    
    for(auto bufferItem: buffer_)
    {
        EXPECT_FALSE((*bufferItem));
    }
}

TEST_F(ProducerConsumerTest, WhenAddingAVeryFastProducerAndSomeSlowConsumersInAnEmptySharedBuffer_ThenAfterWaitingTheSharedBufferIsFull)
{
    const size_t BUFFER_SIZE = 50;
    const uint64_t DELAY_PRODUCER = 20;
    const size_t NUMBER_PRODUCERS = 1;
    const uint64_t DELAY_CONSUMERS = 600;
    const size_t NUMBER_CONSUMERS = 4;
    
    addElementsToBuffer(BUFFER_SIZE);
    IPC::start(buffer_);
    
    PC_Params params(NUMBER_PRODUCERS, NUMBER_CONSUMERS, DELAY_PRODUCER, DELAY_CONSUMERS);
    createProducersAndConsumers(params);

    EXPECT_TRUE(waitForIndexValue(BUFFER_SIZE, DELAY_PRODUCER));
    IPC::stop();
}

TEST_F(ProducerConsumerTest, WhenAddingSeveralProducersThatFillABuffer_ThenAfterRemovingAllProducersAndAddinConsumersTheBufferIsEmpty)
{
    const size_t BUFFER_SIZE = 10;
    const uint64_t DELAY = 20;
    const size_t NUMBER_PRODUCERS = 5;
    const size_t NUMBER_CONSUMERS = 5;

    addElementsToBuffer(BUFFER_SIZE);
    IPC::start(buffer_);

    //Adding only producers
    PC_Params params(NUMBER_PRODUCERS, 0, DELAY, 0);
    createProducersAndConsumers(params);
    
    EXPECT_TRUE(waitForIndexValue(BUFFER_SIZE, DELAY));

    //Now remove all producers and add some consumers
    IPC::removeProducers();
    params = PC_Params(0, NUMBER_CONSUMERS, 0, DELAY);
    createProducersAndConsumers(params);

    EXPECT_TRUE(waitForIndexValue(0, DELAY));

    IPC::stop();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
