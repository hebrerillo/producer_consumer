#ifndef PC_CONSUMER_H
#define PC_CONSUMER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class SharedBuffer;

class Consumer
{
public:

    /**
     * @param[in/out] buffer The buffer where the consumer will extract values from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    Consumer(SharedBuffer* buffer, const std::chrono::milliseconds& delay);

    /**
     * @return Whether this consumer is running.
     */
    bool isRunning() const;

    /**
     * Stops this consumer from consuming elements from the buffer.
     */
    void stop();

    ~Consumer();
private:

    /**
     * Make this consumer to rest by sleeping 'delay' milliseconds, unless 'quitSignal_' is raised, in which case
     * the call returns immediately.
     *
     * @param[in] delay The amount of milliseconds that this consumer will sleep.
     * @return true if this consumer slept 'delay' milliseconds, false if 'quitSignal_' was raised while the sleeping time.
     */
    bool rest(const std::chrono::milliseconds& delay);

    /**
     * Starts extracting elements from the buffer 'buffer'.
     *
     * @param[in/out] buffer The buffer to extract elements from.
     * @param[in] delay The delay that the consumer will take after consuming one item.
     */
    void run(SharedBuffer* buffer, const std::chrono::milliseconds& delay);

    std::thread thread_;
    std::atomic<bool> quitSignal_;
    std::mutex mutex_;
    std::condition_variable stopCV_;
};

#endif