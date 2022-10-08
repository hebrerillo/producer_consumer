#include <iostream>
#include "sharedBuffer.h"
#include "producer.h"
#include "consumer.h"

SharedBuffer::SharedBuffer(const std::vector<IBufferItem* >& buffer)
: currentIndex_(0)
, buffer_(buffer)
, quitSignal_(false)
{}

void SharedBuffer::push(const Producer* producer)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (currentIndex_ < buffer_.size())
    {
        buffer_[currentIndex_++]->fill();
        std::cout << "Pushing value" << std::endl;
        quitCV_.notify_all();
    }
    else
    {
        std::cout << "Buffer full. Waiting for someone to consume." << std::endl;
        quitCV_.wait(lock, [this, producer](){
            return currentIndex_ < buffer_.size() || quitSignal_.load() || !producer->isRunning();
        });
    }
    lock.unlock();
}

void SharedBuffer::pop(const Consumer* consumer)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (currentIndex_ > 0)
    {
        buffer_[--currentIndex_]->empty();
        std::cout << "Poping value" << std::endl;
        quitCV_.notify_all();
    }
    else
    {
        std::cout << "Buffer empty. Waiting for someone to push." << std::endl;
        quitCV_.wait(lock, [this, consumer](){
            return (currentIndex_ > 0) || quitSignal_.load() || !consumer->isRunning();
        });
    }
    lock.unlock();
}

void SharedBuffer::stop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    quitSignal_.exchange(true); //If the signaling is performed without locking, Helgrind complains that the lock associated with 'quitSignal' is not held by any thread.
    quitCV_.notify_all();
    lock.unlock();

    removeProducers();
    removeConsumers();
}

bool SharedBuffer::isRunning() const
{
    return !quitSignal_.load();
}

void SharedBuffer::addProducer()
{
    producers_.push_back(new Producer(this, std::chrono::milliseconds(500)));
}

void SharedBuffer::addConsumer()
{
    consumers_.push_back(new Consumer(this, std::chrono::milliseconds(500)));
}

void SharedBuffer::removeConsumer()
{
    std::list<Consumer* >::iterator consumerIterator = consumers_.begin();
    if (consumerIterator == consumers_.end())
    {
        return;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    (*consumerIterator)->stop();
    quitCV_.notify_all();
    lock.unlock();

    delete (*consumerIterator);
    consumers_.erase(consumerIterator);
}

void SharedBuffer::removeProducer()
{
    std::list<Producer* >::iterator producer = producers_.begin();
    if (producer == producers_.end())
    {
        return;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    (*producer)->stop();
    quitCV_.notify_all();
    lock.unlock();

    delete (*producer);
    producers_.erase(producer);
}

void SharedBuffer::removeConsumers()
{
    while(!consumers_.empty())
    {
        removeConsumer();
    }
}

void SharedBuffer::removeProducers()
{
    while(!producers_.empty())
    {
        removeProducer();
    }
}
