#include <gtest/gtest.h>
#include "manager.h"
#include "bufferItem.h"

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
        ProducerConsumerManager* manager; //The manager of producers and consumers.
        PC_Params(size_t _numberOfProducers, size_t _numberOfConsumers, uint64_t _producersDelay, uint64_t _consumersDelay, ProducerConsumerManager* _manager)
        : numberOfProducers(_numberOfProducers)
        , numberOfConsumers(_numberOfConsumers)
        , producersDelay(_producersDelay)
        , consumersDelay(_consumersDelay)
        , manager(_manager)
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
     * Add a number of producers and consumers to a ProducerConsumerManager instance.
     *
     * @param[in] numberOfProducers The number of producers to be added.
     * @param[in] numberOfConsumers The number of consumers to be added.
     * @param[out] manager The manager that will add the producers and consumers.
     */
    void createProducersAndConsumers(const PC_Params& params);

    SharedBuffer::ItemsBuffer buffer_;
    unsigned long leaked, dubious, reachable, suppressed;
    unsigned long finalLeaked, finalDubious, finalReachable, finalSuppressed;
    ValgrindCheck valgrindCheck_;
};