#include <map>
#include <list>
#include <string>
#include "LRUCacheItem.h"
#include "SqliteDiskCache.h"

#ifndef LRU_CACHE
#define LRU_CACHE

class LRUCache
{
private:
    std::map<std::string, LRUCacheItem *> mMap;
    std::list<std::string> mQueue;
    int mMaxCache;
    IDiskCache *mpDiskCache;

public:
    /// Takes Ownership of PDiskCache
    LRUCache(IDiskCache *pDiskCache, int maxCache = 10)
    {
        mpDiskCache = pDiskCache;
        mMaxCache = maxCache;
    }

    ~LRUCache()
    {
        while (mQueue.size() > mMaxCache)
        {
            std::string backItem = mQueue.back();
            LRUCacheItem *pCacheItem = mMap[backItem];

            delete pCacheItem;

            mMap.erase(backItem);
            mQueue.pop_back();
        }

        delete mpDiskCache;
    }

    LRUCacheItem *get(std::string identifier)
    {
        std::map<std::string, LRUCacheItem *>::iterator search = mMap.find(identifier);

        if (search != mMap.end())
        {
            reorder_map(identifier);
            return search->second;
        }
        else
        {
            LRUCacheItem *pItem = mpDiskCache->get(identifier);
            if (pItem != NULL)
            {
                mMap[identifier] = pItem;
                mQueue.push_front(identifier);
                try_evict();
            }
            return pItem;
        }

        return NULL;
    }

    void put(std::string identifier, LRUCacheItem *pItem)
    {
        mMap[identifier] = pItem;
        mQueue.push_front(identifier);
        try_evict();
    }

    // TEST HOOKS
    std::string get_first()
    {
        return mQueue.front();
    }

    std::string get_last()
    {
        return mQueue.back();
    }

    int get_count()
    {
        return mQueue.size();
    }

private:
    void try_evict()
    {
        while (mQueue.size() > mMaxCache)
        {
            std::string backItem = mQueue.back();
            LRUCacheItem *pCacheItem = mMap[backItem];

            if (pCacheItem->get_is_dirty())
            {
                mpDiskCache->put(pCacheItem);
            }

            delete pCacheItem;

            mMap.erase(backItem);
            mQueue.pop_back();
        }
    }

    void reorder_map(std::string identifier)
    {
        mQueue.remove(identifier);
        mQueue.push_front(identifier);
    }
};

#endif