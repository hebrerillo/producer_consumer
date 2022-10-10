#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "manager.h"
#include "main.h"

#define DEFAULT_DELAY 500       //The delay that produces and consumers will take after producing and consuming an element, respectively.
#define DEFAULT_BUFFER_SIZE 20


static void addBufferElements(SharedBuffer::ItemsBuffer& buffer)
{
    for(size_t i = 0; i < DEFAULT_BUFFER_SIZE; i++)
    {
        buffer.push_back(new BufferItem);
    }
}

static void destroyBufferElements(SharedBuffer::ItemsBuffer& buffer)
{
    for(auto itemBuffer: buffer)
    {
        delete itemBuffer;
    }
}

int main()
{
    SharedBuffer::ItemsBuffer buffer;
    addBufferElements(buffer);
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

    destroyBufferElements(buffer);

    return 0;
}