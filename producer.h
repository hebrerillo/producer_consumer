#ifndef PC_PRODUCER_H
#define PC_PRODUCER_H

#include <thread>
#include <atomic>

class SharedBuffer;

class Producer
{
public:

    /**
     * Constructor
     *
     * @param[in/out] buffer The buffer where the producer will insert values.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    Producer(SharedBuffer* buffer, const std::chrono::milliseconds& delay);

    /**
     * @return Whether this producer is running.
     */
    bool isRunning() const;

    /**
     * Stops this producer from producing elements to the buffer.
     */
    void stop();

    ~Producer();
private:

    /**
     * Starts adding elements to the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to insert elements to.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    void run(SharedBuffer* buffer, const std::chrono::milliseconds& delay);

    std::thread thread_;
    std::atomic<bool> quitSignal_;
};

#endif