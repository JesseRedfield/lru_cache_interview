#ifndef LRU_CACHE_ITEM
#define LRU_CACHE_ITEM

/// For brevity this is implemented to only work on strings.
/// If I had more time LRUCacheItem would be more extensible.
/// I would definitely either make it a template class or continue with the virtual pattern.

// The idea is to serialize/deserialize off of disk, either using extensions like json or XML
// When deserializing from a database load, this class will not be dirty, but if a value is set using setValue
// then the dirty flag would  be set.

class LRUCacheItem
{
protected:
    std::string mCacheId;
    std::string mValue;
    bool mbIsDirty;

public:
    LRUCacheItem(std::string cacheId)
    {
        mCacheId.assign(cacheId);
        mbIsDirty = false;
    }

    ~LRUCacheItem()
    {
    }

    LRUCacheItem(std::string cacheId, std::string value)
    {
        mCacheId.assign(cacheId);
        mValue.assign(value);
        mbIsDirty = true;
    }

    virtual void set_value(std::string value)
    {
        mValue.assign(value);
        mbIsDirty = true;
    }

    virtual std::string get_value() const
    {
        return mValue;
    }

    virtual std::string get_cache_id() const
    {
        return mCacheId;
    }

    virtual bool get_is_dirty() const
    {
        return mbIsDirty;
    }

    virtual std::string serialize() const
    {
        return mValue;
    }

    virtual bool deserialize(std::string data)
    {
        mValue.assign(data);
        mbIsDirty = false;
        return true;
    }
};

#endif