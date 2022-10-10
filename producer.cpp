#include "producer.h"
#include "sharedBuffer.h"

Producer::Producer(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
: quitSignal_(false)
{
    thread_ = std::thread(&Producer::run, this, buffer, delay);
}

bool Producer::isRunning() const
{
    return !quitSignal_.load();
}

void Producer::stop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    quitSignal_.exchange(true);
    stopCV_.notify_all();
}

void Producer::run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
{
    while(buffer->isRunning() && rest(delay))
    {
        buffer->produce(this);
    }
}

bool Producer::rest(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto quitPredicate = [this]()
    {
        return quitSignal_.load();
    };

    return !stopCV_.wait_for(lock, delay, quitPredicate);
}

Producer::~Producer()
{
    thread_.join();
}