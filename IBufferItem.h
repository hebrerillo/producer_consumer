/**
 * Class that represents an item that can be inserted in the shared buffer.
 * The item can be filled by a producer by calling 'fill', or it can be emptied by a consumer by calling 'empty'.
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