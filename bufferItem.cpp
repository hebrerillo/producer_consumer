#include "bufferItem.h"

BufferItem::BufferItem()
: value_(false)
{}

void BufferItem::fill()
{
    assert(!value_);
    value_ = true;
}

void BufferItem::empty()
{
    assert(value_);
    value_ = false;
}

BufferItem::operator bool() const
{
    return value_;
}