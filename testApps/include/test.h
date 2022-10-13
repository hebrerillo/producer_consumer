#include <gtest/gtest.h>
#include "IPC.h"

#include "valgrind_check.h"

class ProducerConsumerTest: public ::testing::Test { 
public: 

    /**
     * A convenient structure to be passed to methods to avoid a high number of parameters.
     */
    struct PC_Params
    {
        size_t numberOfProducers; //The number of producers
        size_t numberOfConsumers; //The number of consumers
        uint64_t producersDelay; //The delay that producers will take after producing an item.
        uint64_t consumersDelay; //The delay that consumers will take after consuming an item.
        PC_Params(size_t _numberOfProducers, size_t _numberOfConsumers, uint64_t _producersDelay, uint64_t _consumersDelay)
        : numberOfProducers(_numberOfProducers)
        , numberOfConsumers(_numberOfConsumers)
        , producersDelay(_producersDelay)
        , consumersDelay(_consumersDelay)
        {
        }
    };

    void SetUp() override;

    void TearDown() override;

protected:

    /**
     * Add 'size' elements to the buffer 'buffer_'. It also fills the first 'numberOfFilledElements' elements.
     *
     * @param[in] size The number of elements to add.
     * @param[in] numberOfFilledElements The number of elements that will be filled
     */
    void addElementsToBuffer(size_t size, size_t numberOfFilledElements = 0);

    /**
     * Add a number of producers and consumers.
     *
     * @param[in] numberOfProducers The number of producers to be added.
     * @param[in] numberOfConsumers The number of consumers to be added.
     */
    void createProducersAndConsumers(const PC_Params& params);

    /**
     * The method checks the index value of the shared buffer every delay/2 milliseconds, with a
     * maximum number of tries of buffer_ * delay * 2.
     *
     * @param[in] indexValue The desired index value that the buffer should have
     * @param[in] delay The expected delay that producers and/or consumers will have.
     * @return true if the current index value of the shared buffer reaches 'indexValue', false otherwise.
     */
    bool waitForIndexValue(size_t indexValue, uint64_t delay);

    IPC::ItemsBuffer buffer_;
    unsigned long leaked, dubious, reachable, suppressed;
    unsigned long finalLeaked, finalDubious, finalReachable, finalSuppressed;
    ValgrindCheck valgrindCheck_;
};