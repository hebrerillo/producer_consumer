#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>
#include <assert.h>
#include <atomic>

class Producer;
class Consumer;

/**
 * Class that represents an item that can be inserted in the shared buffer.
 * The item can be filled by a producir by calling 'fill', or it can be emptied by a consumer by calling 'empty'.
 */
class IBufferItem
{
public:
    /**
     * Fills this item.
     */
    virtual void fill() = 0;

    /**
     * Empties this item.
     */
    virtual void empty() = 0;

    /**
     * @return true if the item is filled, false if the item is empty.
     */
    virtual operator bool () const = 0;

    virtual ~IBufferItem(){}
};

class BufferItem: public IBufferItem
{
public:
    BufferItem()
    : value_(false)
    {}

    void fill() override
    {
        assert(!value_);
        value_ = true;
    }

    void empty() override
    {
        assert(value_);
        value_ = false;
    }

    operator bool() const override
    {
        return value_;
    }

private:
    bool value_;
};

/**
 * Class that represents the shared buffer between producers and consumers.
 * 
 */
class SharedBuffer
{
public:

    /**
     * Constructor
     *
     * @param[int/out] buffer The buffer to produce and consume items. 
     * @note Important!! All the items in the buffer should be empty.
     */
    SharedBuffer(const std::vector<IBufferItem* >& buffer);

    /**
     * Adds an element to the buffer in the 'currentIndex_' position and increases 'currentIndex_'. This is the producer role.
     *
     * @param[in] producer The producer.
     */
    void push(const Producer* producer);

    /**
     * Extracts the element 'currentIndex_' from the buffer and decreases 'currentIndex_'. This is the consumer role.
     *
     * @param[in] consumer The consumer
     */
    void pop(const Consumer* consumer);

    /**
     * Stops the buffer from accepting and/or returning elements.
     */
    void stop();

    /**
     * Whether the buffer is accepting and returning elements.
     */
    bool isRunning() const;

private:
    size_t currentIndex_; //The index of the last produced item.
    std::vector<IBufferItem* > buffer_;
    std::mutex mutex_; //To syncrhonize accesses to 'buffer_' and 'currentIndex_'.
    std::condition_variable quitCV_;
    std::atomic<bool> quitSignal_;
};