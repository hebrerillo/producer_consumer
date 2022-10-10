#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "manager.h"
#include "IBufferItem.h"

#define DEFAULT_DELAY 500       //The delay that produces and consumers will take after producing and consuming an element, respectively.
#define DEFAULT_BUFFER_SIZE 20

/**
 * An implementation of an item that is suitable to be produced and consumed in the shared buffer.
 */
class BufferItem: public IBufferItem
{
public:
    BufferItem()
    : value_(false)
    {}

    void fill() override
    {
        assert(!value_);
        value_ = true;
    }

    void empty() override
    {
        assert(value_);
        value_ = false;
    }

private:
    bool value_;
};


int main()
{
    SharedBuffer::ItemsBuffer buffer;
    for(size_t i = 0; i < DEFAULT_BUFFER_SIZE; i++)
    {
        buffer.push_back(new BufferItem);
    }

    ProducerConsumerManager manager(buffer);

    int input = -1;
    while (input != 0)
    {
        std::cin >> input;
        switch(input)
        {
            case 1:
                manager.addProducer(std::chrono::milliseconds(DEFAULT_DELAY));
                break;
            case 2:
                manager.addConsumer(std::chrono::milliseconds(DEFAULT_DELAY));
                break;
            case 3:
                manager.removeProducer();
                break;
            case 4:
                manager.removeConsumer();
                break;
            case 5:
                manager.removeProducers();
                break;
            case 6:
                manager.removeConsumers();
                break;
            case 7:
                manager.removeConsumers();
                manager.removeProducers();
                break;
            case 8:
                for(size_t i = 0; i < 5; ++i)
                {
                    manager.addProducer(std::chrono::milliseconds(DEFAULT_DELAY));
                }
                break;
            case 9:
                for(size_t i = 0; i < 5; ++i)
                {
                    manager.addConsumer(std::chrono::milliseconds(DEFAULT_DELAY));
                }
                break;
        };
    }

    manager.stop();

    for(auto itemBuffer: buffer)
    {
        delete itemBuffer;
    }

    return 0;
}