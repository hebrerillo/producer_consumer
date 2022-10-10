#include "consumer.h"
#include "sharedBuffer.h"

Consumer::Consumer(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
: quitSignal_(false)
{
    thread_ = std::thread(&Consumer::run, this, buffer, delay);
}

bool Consumer::isRunning() const
{
    return !quitSignal_.load();
}

void Consumer::stop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    quitSignal_.exchange(true);
    stopCV_.notify_all();
}

void Consumer::run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
{
    while(buffer->isRunning() && rest(delay))
    {
        buffer->consume(this);
    }
}

bool Consumer::rest(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto quitPredicate = [this]()
    {
        return quitSignal_.load();
    };

    return !stopCV_.wait_for(lock, delay, quitPredicate);
}

Consumer::~Consumer()
{
    thread_.join();
}