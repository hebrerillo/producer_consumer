#ifndef PC_I_ACTOR_H
#define PC_I_ACTOR_H

#include <thread>
#include <mutex>
#include <condition_variable>

class SharedBuffer;

/**
 * Class that represents an entity or actor that can interact with the shared buffer, like a producer or a consumer.
 *
 * A producer interacts with the buffer by producing items in it.
 * A consumer interacts with the buffer by consuming items from it.
 */
class IBufferActor
{
public:

    /**
     * Constructor
     * 
     * @param[in/out] sharedBuffer The buffer with which this actor will interact.
     */
    IBufferActor(SharedBuffer* sharedBuffer);

    /**
     * This actor starts to interact with the buffer 'buffer_' by starting the thread 'thread_' and calling 'run'.
     *
     * @param[in] delay The delay that this actor will take after interacting with the buffer.
     */
    void start(const std::chrono::milliseconds& delay);

    /**
     * Stops this actor from interacting with the shared buffer 'buffer_'.
     */
    void stop();

    /**
     * @return Whether this actor is running.
     */
    bool isRunning() const;

    virtual ~IBufferActor(){}

protected:

    /**
     * Place this actor to rest by sleeping 'delay' milliseconds, unless 'quitSignal_' is raised, in which case
     * the call returns immediately.
     *
     * @param[in] delay The amount of milliseconds that this actor will sleep.
     * @return true if this actor was capable of sleeping 'delay' milliseconds, false if 'quitSignal_' was raised while the sleeping time.
     */
    bool rest(const std::chrono::milliseconds& delay);
    
    SharedBuffer* sharedBuffer_; //The buffer that this actor will interact with.

private:

    /**
     * This is the asynchronous method that this actor will execute to interact with the shared buffer 'sharedBuffer_'.
     * This method will be executed until 'quitSignal_' is raised or until 'sharedBuffer_' is stopped.
     *
     * @param[in] delay The delay that this actor will take after interacting with the buffer.
     */
    virtual void run(const std::chrono::milliseconds& delay) = 0;

    std::thread thread_;
    bool quitSignal_;
    std::mutex mutex_;
    std::condition_variable stopCV_;
};

#endif
