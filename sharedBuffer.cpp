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
}

bool SharedBuffer::isRunning() const
{
    return !quitSignal_.load();
}