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
    SharedBuffer::ItemsBuffer buffer;
    addBufferElements(buffer);
    ProducerConsumerManager manager(buffer);

    showMenu();
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
                break;
            case 7:
                for(size_t i = 0; i < 5; ++i)
                {
                    manager.addProducer(std::chrono::milliseconds(DEFAULT_DELAY));
                }
                break;
            case 8:
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