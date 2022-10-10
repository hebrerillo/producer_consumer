#ifndef PC_PRODUCER_H
#define PC_PRODUCER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

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
     * Make this producer to rest by sleeping 'delay' milliseconds, unless 'quitSignal_' is raised, in which case
     * the call returns immediately.
     *
     * @param[in] delay The amount of milliseconds that this producer will sleep.
     * @return true if this producer slept 'delay' milliseconds, false if 'quitSignal_' was raised while the sleeping time.
     */
    bool rest(const std::chrono::milliseconds& delay);

    /**
     * Starts adding elements to the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to insert elements to.
     * @param[in] delay The delay that the producer will take after producing one item.
     */
    void run(SharedBuffer* buffer, const std::chrono::milliseconds& delay);

    std::thread thread_;
    std::atomic<bool> quitSignal_;
    std::mutex mutex_;
    std::condition_variable stopCV_;
};

#endif