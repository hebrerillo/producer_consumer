#ifndef PC_BUFFER_ITEM_H
#define PC_BUFFER_ITEM_H

#include <assert.h>
#include "IBufferItem.h"

/**
 * An implementation of an item that is suitable to be produced and consumed in the shared buffer.
 */
class BufferItem: public IBufferItem
{
public:

    /**
     * Constructor.
     *
     * @param[filled] Whether this item is filled.
     */
    explicit BufferItem(bool filled = false);

    /**
     * Sets this object as filled (or produced by a producer).
     */
    void fill() override;

    /**
     * Sets this object as empty (or consumed by a consumer).
     */
    void empty() override;

    operator bool() const override;

private:
    bool value_;
};

#endif