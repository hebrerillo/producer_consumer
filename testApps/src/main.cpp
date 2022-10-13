/**
 * An executable shell with commands to test the producer-consumer implementation.
 */
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "IPC.h"
#include "bufferItem.h"

#define DEFAULT_DELAY 500       //The delay that produces and consumers will take after producing and consuming an element, respectively.
#define DEFAULT_BUFFER_SIZE 20

/**
 * Add buffer items into 'buffer'.
 *
 * @param buffer The buffer to add items into.
 */
static void addBufferElements(IPC::ItemsBuffer& buffer)
{
    for(size_t i = 0; i < DEFAULT_BUFFER_SIZE; i++)
    {
        buffer.push_back(new BufferItem);
    }
}

/**
 * Remove buffer items from 'buffer'.
 *
 * @param buffer The buffer to remove items from.
 */
static void destroyBufferElements(IPC::ItemsBuffer& buffer)
{
    for(auto itemBuffer: buffer)
    {
        delete itemBuffer;
    }

    buffer.clear();
}

static void showMenu()
{
    std::cout << "The following commands will be kindly accepted: " << std::endl << std::endl;
    std::cout << "0: Quits the application. " << std::endl;
    std::cout << "1: Add a producer. " << std::endl;
    std::cout << "2: Add a consumer. " << std::endl;
    std::cout << "3: Remove a producer. " << std::endl;
    std::cout << "4: Remove a consumer. " << std::endl;
    std::cout << "5: Remove all producers. " << std::endl;
    std::cout << "6: Remove all consumers. " << std::endl;
    std::cout << "7: Add 5 producers. " << std::endl;
    std::cout << "8: Add 5 consumers. " << std::endl;
}

int main()
{
    IPC::ItemsBuffer buffer;
    addBufferElements(buffer);
    IPC::start(buffer);

    showMenu();
    int input = -1;
    while (input != 0)
    {
        std::cin >> input;
        switch(input)
        {
            case 1:
                IPC::addProducer(std::chrono::milliseconds(DEFAULT_DELAY));
                break;
            case 2:
                IPC::addConsumer(std::chrono::milliseconds(DEFAULT_DELAY));
                break;
            case 3:
                IPC::removeProducer();
                break;
            case 4:
                IPC::removeConsumer();
                break;
            case 5:
                IPC::removeProducers();
                break;
            case 6:
                IPC::removeConsumers();
                break;
            case 7:
                for(size_t i = 0; i < 5; ++i)
                {
                    IPC::addProducer(std::chrono::milliseconds(DEFAULT_DELAY));
                }
                break;
            case 8:
                for(size_t i = 0; i < 5; ++i)
                {
                    IPC::addConsumer(std::chrono::milliseconds(DEFAULT_DELAY));
                }
                break;
        };
    }

    IPC::stop();

    destroyBufferElements(buffer);

    return 0;
}