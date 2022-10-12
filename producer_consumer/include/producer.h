#ifndef PC_PRODUCER_H
#define PC_PRODUCER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "IActor.h"

class Producer : public IBufferActor
{
public:

    /**
     * Constructor.
     *
     * @param[in/out] buffer The buffer where the producer will insert values.
     */
    explicit Producer(SharedBuffer* buffer);

private:


    /**
     * Starts adding elements to the buffer.
     *
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    void run(const std::chrono::milliseconds& delay) override;

};

#endif