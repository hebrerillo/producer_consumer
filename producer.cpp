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
    quitSignal_.exchange(true);
}

void Producer::run(SharedBuffer* buffer, const std::chrono::milliseconds& delay)
{
    while(buffer->isRunning() && !quitSignal_.load())
    {
        buffer->produce(this);
        std::this_thread::sleep_for(delay);
    }
}

Producer::~Producer()
{
    thread_.join();
}