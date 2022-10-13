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
     * Sets the buffer that will be shared among producers and consumers. It also allow the internal buffer 'sharedBuffer_' to start accepting consumers and producers.
     *
     * @param[in] buffer The shared buffer.
     * @note This method should be followed by a call to stop. Calling this method twice without a call to stop will cause undefined behaviour.
     */
    static void start(const IPC::ItemsBuffer& buffer);

    /**
     * Adds a producer to produce items into the buffer 'buffer_'.
     *
     * @param[in] delay The delay the producer will take after producing an element.
     */
    static void addProducer(const std::chrono::milliseconds& delay);

    /**
     * Adds a consumer to consume items from 'buffer_'.
     *
     * @param[in] delay The delay the consumer will take after consuming an element.
     */
    static void addConsumer(const std::chrono::milliseconds& delay);

    /**
     * Removes a consumer.
     */
    static void removeConsumer();

    /**
     * Removes a producer.
     */
    static void removeProducer();

    /**
     * Removes all consumers.
     */
    static void removeConsumers();

    /**
     * Removes all producers.
     */
    static void removeProducers();

    /**
     * Stops the shared buffer, the producers and the consumers.
     */
    static void stop();

    /**
     * @return The index of the next item to be filled in the buffer.
     */
    static size_t getCurrentIndex() ;

private:

    /**
     * Removes a consumer iterator from the 'consumers_' list. It also stops the 'Consumer' object associated with the itarator and frees its memory.
     *
     * @param[in/out] consumerIterator The consumer iterator to be destroyed.
     */
    static void removeConsumer(const ConsumerIterator& consumerIterator);

    /**
     * Removes a producer iterator from the 'producers_' list. It also stops the 'Producer' object associated with the itarator and frees its memory.
     *
     * @param[in/out] producerIterator The producer iterator to be destroyed.
     */
    static void removeProducer(const ProducerIterator& producerIterator);

    static SharedBuffer* sharedBuffer_;
    static std::list<Consumer* > consumers_;
    static std::list<Producer* > producers_;
    static std::mutex mutexConsumers_; //Synchronizes accesses to 'consumers_'
    static std::mutex mutexProducers_; //Synchronizes accesses to 'producers_'
};

#endif