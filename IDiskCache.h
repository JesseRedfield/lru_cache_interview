#include "LRUCacheItem.h"

#ifndef IDISK_CACHE
#define IDISK_CACHE

class IDiskCache
{
public:
    virtual bool initialize(const char *path) = 0;

    virtual void put(LRUCacheItem *pItem) = 0;

    virtual LRUCacheItem *get(std::string identifier) = 0;

    virtual ~IDiskCache(){};
};

#endif