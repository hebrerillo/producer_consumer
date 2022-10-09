#include "manager.h"

ProducerConsumerManager::ProducerConsumerManager(const SharedBuffer::ItemsBuffer& buffer)
: sharedBuffer_(buffer)
{}

void ProducerConsumerManager::addProducer(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutexProducers_);
    if (!sharedBuffer_.isRunning())
    {
        return;
    }
    producers_.push_back(new Producer(&sharedBuffer_, delay));
}

void ProducerConsumerManager::addConsumer(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutexConsumers_);
    if (!sharedBuffer_.isRunning())
    {
        return;
    }
    consumers_.push_back(new Consumer(&sharedBuffer_, delay));
}

void ProducerConsumerManager::removeConsumer()
{
    std::unique_lock<std::mutex> lock(mutexConsumers_);
    ConsumerIterator consumerIterator = consumers_.begin();
    if (consumerIterator == consumers_.end())
    {
        return;
    }

    removeConsumer(consumerIterator);
}

void ProducerConsumerManager::removeConsumer(const ConsumerIterator& consumerIterator)
{
    if (consumerIterator == consumers_.end())
    {
        return;
    }

    Consumer* consumer = *(consumerIterator);
    consumer->stop();
    sharedBuffer_.notify();
    consumers_.erase(consumerIterator);
    delete consumer;
}

void ProducerConsumerManager::removeProducer()
{
    std::unique_lock<std::mutex> lock(mutexProducers_);
    ProducerIterator producerIterator = producers_.begin();
    if (producerIterator == producers_.end())
    {
        return;
    }

    removeProducer(producerIterator);
}

void ProducerConsumerManager::removeProducer(const ProducerIterator& producerIterator)
{
    if (producerIterator == producers_.end())
    {
        return;
    }

    Producer* producer = *(producerIterator);
    producer->stop();
    sharedBuffer_.notify();
    producers_.erase(producerIterator);
    delete producer;
}

void ProducerConsumerManager::removeConsumers()
{
    std::unique_lock<std::mutex> lock(mutexConsumers_);
    while(!consumers_.empty())
    {
        removeConsumer(consumers_.begin());
    }
}

void ProducerConsumerManager::removeProducers()
{
    std::unique_lock<std::mutex> lock(mutexProducers_);
    while(!producers_.empty())
    {
        removeProducer(producers_.begin());
    }
}

void ProducerConsumerManager::stop()
{
    removeProducers();
    removeConsumers();
    sharedBuffer_.stop();
}
