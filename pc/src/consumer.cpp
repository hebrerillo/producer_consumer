#include "consumer.h"
#include "sharedBuffer.h"

Consumer::Consumer(SharedBuffer* buffer)
: IBufferActor(buffer)
{}


void Consumer::run(const std::chrono::milliseconds& delay)
{
    while(sharedBuffer_->isRunning() && rest(delay))
    {
        sharedBuffer_->consume(this);
    }
}
