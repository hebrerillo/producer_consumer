#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string.h>
#include <condition_variable>
#include <vector>
#include <list>
#include "sharedBuffer.h"
#include "producer.h"
#include "consumer.h"

/**
 * Class to manage all the producers and consumers.
 */
class ProducersConsumersManager
{
public:

    /**
     * Constructor
     * @param[in/out] sharedBuffer The buffer shared among producers and consumers.
     */
    ProducersConsumersManager(SharedBuffer* sharedBuffer)
    : sharedBuffer_(sharedBuffer)
    {
        addProducer();
        addConsumer();
    }

    /**
     * Adds a producer to produce items into the buffer 'sharedBuffer_'.
     */
    void addProducer()
    {
        producers_.push_back(new Producer(sharedBuffer_, std::chrono::milliseconds(500)));
    }

    /**
     * Adds a consumer to consume items from the buffer 'sharedBuffer_'.
     */
    void addConsumer()
    {
        consumers_.push_back(new Consumer(sharedBuffer_, std::chrono::milliseconds(500)));
    }

    /**
     * Removes a consumer.
     */
    void removeConsumer()
    {
        std::list<Consumer* >::iterator consumer = consumers_.begin();
        if (consumer == consumers_.end())
        {
            return;
        }
        (*consumer)->stop();
        consumers_.erase(consumer);
        delete (*consumer);
    }

    /**
     * Removes a producer.
     */
    void removeProducer()
    {
        std::list<Producer* >::iterator producer = producers_.begin();
        if (producer == producers_.end())
        {
            return;
        }
        (*producer)->stop();
        producers_.erase(producer);
        delete (*producer);
    }

    ~ProducersConsumersManager()
    {
        for(auto consumer: consumers_)
        {
            consumer->stop();
            delete consumer;
        }

        for(auto producer: producers_)
        {
            producer->stop();
            delete producer;
        }
    }
private:
    SharedBuffer* sharedBuffer_;
    std::list<Consumer* > consumers_;
    std::list<Producer* > producers_;
};

int main()
{
    std::vector<IBufferItem* > buffer;
    for(size_t i = 0; i < 10; i++)
    {
        buffer.push_back(new BufferItem);
    }

    SharedBuffer sharedBuffer(buffer);
    ProducersConsumersManager manager(&sharedBuffer);

    int input = -1;
    while (input != 0)
    {
        std::cin >> input;
        switch(input)
        {
            case 1:
                manager.addProducer();
                break;
            case 2:
                manager.removeProducer();
                break;
            case 3:
                manager.addConsumer();
                break;
            case 4:
                manager.removeConsumer();
                break;
        };
    }

    sharedBuffer.stop();

    for(auto itemBuffer: buffer)
    {
        delete itemBuffer;
    }
    return 0;
}