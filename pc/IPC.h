#ifndef PC_IPC_H
#define PC_IPC_H

#include <chrono>
#include <vector>
#include "IBufferItem.h"

/**
 * The interface class to manage producers and consumers
 */
class IPC
{
public:
    using ItemsBuffer = std::vector<IBufferItem* >; //A type representing the buffer of items shared among producers and consumers.

    /**
     * Sets the buffer that will be shared among producers and consumers. It also allow the internal buffer to start accepting consumers and producers.
     *
     * @param[in] buffer The shared buffer.
     * @note This method should be followed by a call to stop. Calling this method twice without calling stop will cause undefined behaviour.
     */
    static void start(const ItemsBuffer& buffer);

    /**
     * Adds a producer to produce items into the buffer.
     *
     * @param[in] delay The delay the producer will take after producing an element.
     */
    static void addProducer(const std::chrono::milliseconds& delay);

    /**
     * Adds a consumer to consume items from the buffer.
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
    static size_t getCurrentIndex();
};

#endif