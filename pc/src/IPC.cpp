#include "IPC.h"
#include "manager.h"

void IPC::start(const ItemsBuffer& buffer)
{
    ProducerConsumerManager::start(buffer);
}

void IPC::addProducer(const std::chrono::milliseconds& delay)
{
    ProducerConsumerManager::addProducer(delay);
}

void IPC::addConsumer(const std::chrono::milliseconds& delay)
{
    ProducerConsumerManager::addConsumer(delay);
}

void IPC::removeConsumer()
{
    ProducerConsumerManager::removeConsumer();
}

void IPC::removeProducer()
{
    ProducerConsumerManager::removeProducer();
}

void IPC::removeConsumers()
{
    ProducerConsumerManager::removeConsumers();
}

void IPC::removeProducers()
{
    ProducerConsumerManager::removeProducers();
}

void IPC::stop()
{
    ProducerConsumerManager::stop();
}

size_t IPC::getCurrentIndex() 
{
    return ProducerConsumerManager::getCurrentIndex();
}

