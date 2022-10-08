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
    quitSignal_.exchange(true);
}

void Consumer::run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
{
    while(buffer->isRunning() && !quitSignal_.load())
    {
        buffer->consume(this);
        std::this_thread::sleep_for(delay);
    }
}

Consumer::~Consumer()
{
    thread_.join();
}