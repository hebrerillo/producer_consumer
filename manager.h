#ifndef PC_MANAGER_H
#define PC_MANAGER_H

#include <vector>
#include <mutex>
#include "sharedBuffer.h"
#include "producer.h"
#include "consumer.h"

/**
 * Manages the additions and removals of producers and consumers.
 */
class ProducerConsumerManager
{
public:
    using ProducerIterator = std::list<Producer* >::iterator;
    using ConsumerIterator = std::list<Consumer* >::iterator;

    /**
     * Constructor
     *
     * @param[int/out] buffer The buffer to produce and consume items.
     */
    explicit ProducerConsumerManager(const SharedBuffer::ItemsBuffer& buffer);

    /**
     * Adds a producer to produce items into the buffer 'buffer_'.
     *
     * @param[in] delay The delay the producer will take after producing an element.
     */
    void addProducer(const std::chrono::milliseconds& delay);

    /**
     * Adds a consumer to consume items from 'buffer_'.
     *
     * @param[in] delay The delay the consumer will take after consuming an element.
     */
    void addConsumer(const std::chrono::milliseconds& delay);

    /**
     * Removes a consumer.
     */
    void removeConsumer();

    /**
     * Removes a producer.
     */
    void removeProducer();

    /**
     * Removes all consumers.
     */
    void removeConsumers();

    /**
     * Removes all producers.
     */
    void removeProducers();

    /**
     * Stops the shared buffer, the producers and the consumers.
     */
    void stop();

private:

    /**
     * Removes a consumer iterator from the 'consumers_' list. It also stops the 'Consumer' object associated with the itarator and frees its memory.
     *
     * @param[in/out] consumerIterator The consumer iterator to be destroyed.
     */
    void removeConsumer(const ConsumerIterator& consumerIterator);

    /**
     * Removes a producer iterator from the 'producers_' list. It also stops the 'Producer' object associated with the itarator and frees its memory.
     *
     * @param[in/out] producerIterator The producer iterator to be destroyed.
     */
    void removeProducer(const ProducerIterator& producerIterator);

    SharedBuffer sharedBuffer_;
    std::list<Consumer* > consumers_;
    std::list<Producer* > producers_;
    std::mutex mutexConsumers_; //Synchronizes accesses to 'consumers_'
    std::mutex mutexProducers_; //Synchronizes accesses to 'producers_'
};

#endif