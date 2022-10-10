#include "producer.h"
#include "sharedBuffer.h"

Producer::Producer(SharedBuffer* buffer)
: IBufferActor(buffer)
{}

void Producer::run(const std::chrono::milliseconds& delay)
{
    while(sharedBuffer_->isRunning() && rest(delay))
    {
        sharedBuffer_->produce(this);
    }
}
