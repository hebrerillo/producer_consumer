#include "IActor.h"
#include "sharedBuffer.h"

IBufferActor::IBufferActor(SharedBuffer* buffer)
: sharedBuffer_(buffer)
, quitSignal_(false)
{}

void IBufferActor::start(const std::chrono::milliseconds& delay)
{
    thread_ = std::thread(&IBufferActor::run, this, delay);
}

bool IBufferActor::isRunning() const
{
    return !quitSignal_;
}

void IBufferActor::stop()
{
    mutex_.lock();
    quitSignal_ = true;
    sharedBuffer_->notify();
    stopCV_.notify_all();
    mutex_.unlock();

    thread_.join();
}

bool IBufferActor::rest(const std::chrono::milliseconds& delay)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto quitPredicate = [this]()
    {
        return quitSignal_;
    };

    return !stopCV_.wait_for(lock, delay, quitPredicate);
}
