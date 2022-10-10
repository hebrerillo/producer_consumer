#ifndef PC_CONSUMER_H
#define PC_CONSUMER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "IActor.h"

class Consumer : public IBufferActor
{
public:

    /**
     * @param[in/out] buffer The buffer where the consumer will extract values from.
     */
    Consumer(SharedBuffer* buffer);

private:

    /**
     * Starts extracting elements from the buffer.
     *
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    void run(const std::chrono::milliseconds& delay) override;
};

#endif