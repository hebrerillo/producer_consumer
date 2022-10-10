#ifndef PC_MAIN_H
#define PC_MAIN_H
#include <assert.h>
#include "IBufferItem.h"

/**
 * An implementation of an item that is suitable to be produced and consumed in the shared buffer.
 */
class BufferItem: public IBufferItem
{
public:
    BufferItem()
    : value_(false)
    {}

    /**
     * Sets this object as filled (or produced by a producer).
     */
    void fill() override
    {
        assert(!value_);
        value_ = true;
    }

    /**
     * Sets this object as empty (or consumed by a consumer).
     */
    void empty() override
    {
        assert(value_);
        value_ = false;
    }

private:
    bool value_;
};

#endif