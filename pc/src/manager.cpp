#include "manager.h"

SharedBuffer* ProducerConsumerManager::sharedBuffer_;

std::list<Consumer* > ProducerConsumerManager::consumers_;
std::list<Producer* > ProducerConsumerManager::producers_;
std::mutex ProducerConsumerManager::mutexConsumers_;
std::mutex ProducerConsumerManager::mutexProducers_;

void ProducerConsumerManager::start(const IPC::ItemsBuffer& buffer)
{
    sharedBuffer_ = new SharedBuffer(buffer);
}

void ProducerConsumerManager::addProducer(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutexProducers_);
    if (!sharedBuffer_->isRunning())
    {
        return;
    }

    Producer* producer = new Producer(sharedBuffer_);
    producer->start(delay);
    producers_.push_back(producer);
}

void ProducerConsumerManager::addConsumer(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutexConsumers_);
    if (!sharedBuffer_->isRunning())
    {
        return;
    }
    Consumer* consumer = new Consumer(sharedBuffer_);
    consumer->start(delay);
    consumers_.push_back(consumer);
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
    sharedBuffer_->stop();
    delete sharedBuffer_;
}

size_t ProducerConsumerManager::getCurrentIndex()
{
    return sharedBuffer_->getCurrentIndex();
}
