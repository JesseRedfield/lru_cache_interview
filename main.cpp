#include <iostream>
#include <string>
#include "LRUCache.h"
#include "LRUCacheItem.h"
#include "SqliteDiskCache.h"

int main()
{
    std::string cat("cat");
    std::string cat_sound("The Cat Goes Meow");

    std::string dog("dog");
    std::string dog_sound("The Dog Goes Bark");

    std::string cow("cow");
    std::string cow_sound("The Cow Goes Moo");

    std::string chicken("chicken");
    std::string chicken_sound("The Chicken Goes Cluck");

    std::string mouse("mouse");
    std::string mouse_sound("The Mouse Goes Squeak");

    std::string lion("lion");
    std::string lion_sound("The Lion Goes Rawr");

    SqliteDiskCache *pDiskCacheProvider = new SqliteDiskCache();
    pDiskCacheProvider->initialize("cache.db");

    LRUCache cache(pDiskCacheProvider, 3);

    cache.put(cat, new LRUCacheItem(cat, cat_sound));

    // CACHE LIST STATE
    //  1. CAT

    // make sure the first insert works
    printf("\n\nTEST # 1  Insert First Item Into Cache\n");
    printf("##########################################\n");
    if (cache.get_first() != cat)
        printf("Get First Item FAILED\n");
    else
        printf("Get First Item  SUCCESS\n");
    if (cache.get_last() != cat)
        printf("Get Last Item FAILED\n");
    else
        printf("Get Last Item SUCCESS\n");

    // CACHE LIST STATE
    //  1. DOG
    //  2. CAT

    printf("\n\nTEST # 2  Insert Second Item Into Cache\n");
    printf("##########################################\n");
    cache.put(dog, new LRUCacheItem(dog, dog_sound));

    if (cache.get_first() != dog)
        printf("Get First Item: FAILED\n");
    else
        printf("Get First Item: SUCCESS\n");
    if (cache.get_last() != cat)
        printf("Get Last Item: FAILED\n");
    else
        printf("get Last Item: SUCCESS\n");

    // CACHE LIST STATE
    //  1. CHICKEN
    //  2. COW
    //  3. DOG
    //
    //  DISK
    //  - CAT

    printf("\n\nTEST # 3  Insert 2 More Items Cache Eviction Test\n");
    printf("##########################################\n");
    cache.put(cow, new LRUCacheItem(cow, cow_sound));
    cache.put(chicken, new LRUCacheItem(chicken, chicken_sound));
    if (cache.get_first() != chicken)
        printf("Get First Item: FAILED\n");
    else
        printf("Get First Item: SUCCESS\n");
    if (cache.get_last() != dog)
        printf("get Last Item: FAILED\n");
    else
        printf("get Last Item: SUCCESS\n");

    if (cache.get_count() != 3)
        printf("get count: FAILED\n");
    else
        printf("get count: SUCCESS\n");

    // CACHE LIST STATE
    //  1. DOG
    //  2. CHICKEN
    //  3. COW
    //
    //  DISK
    //  - CAT

    printf("\n\nTEST # 4 Get Memory Cached Item\n");
    printf("##########################################\n");
    LRUCacheItem *pMemItem = cache.get(dog);

    if (cache.get_first() != dog)
        printf("Get First Item: FAILED\n");
    else
        printf("Get First Item: SUCCESS\n");
    if (cache.get_last() != cow)
        printf("get Last Item: FAILED\n");
    else
        printf("get Last Item: SUCCESS\n");

    if (cache.get_count() != 3)
        printf("get count: FAILED\n");
    else
        printf("get count: SUCCESS\n");

    if (!pMemItem->get_is_dirty())
        printf("item is dirty: FAILED\n");
    else
        printf("item is dirty: SUCCESS\n");

    // CACHE LIST STATE
    //  1. CAT
    //  2. DOG
    //  3. CHICKEN
    //
    //  DISK
    //  - CAT
    //  - COW
    printf("\n\nTEST # 6 Get Disk Cached Item");
    printf("##########################################\n");
    LRUCacheItem *pDiskItem = cache.get(cat);
    if (cache.get_first() != cat)
        printf("Get First Item: FAILED\n");
    else
        printf("Get First Item: SUCCESS\n");
    if (cache.get_last() != chicken)
        printf("Get Last Item: FAILED\n");
    else
        printf("Get Last Item: SUCCESS\n");

    if (cache.get_count() != 3)
        printf("get count: FAILED\n");
    else
        printf("get count: SUCCESS\n");

    if (pDiskItem->get_is_dirty())
        printf("item is dirty: FAILED\n");
    else
        printf("item is dirty: SUCCESS\n");

    if (pDiskItem->get_value() != cat_sound)
        printf("value check: FAILED\n");
    else
        printf("value check: SUCCESS\n");

    // CACHE LIST STATE
    //  1. CAT
    //  2. COW
    //  3. DOG
    //
    //  DISK
    //  - CAT
    //  - COW
    //  - CHICKEN
    printf("\n\nTEST # 7 Change Mem Item Value And Cache To Disk\n");
    printf("##########################################\n");
    pDiskItem->set_value(lion_sound);

    if (!pDiskItem->get_is_dirty())
        printf("item is dirty: FAILED\n");
    else
        printf("item is dirty: SUCCESS\n");

    cache.get(chicken);
    cache.get(dog);
    cache.get(cow);
    pDiskItem = cache.get(cat);

    if (cache.get_first() != cat)
        printf("Get First Item: FAILED\n");
    else
        printf("Get First Item: SUCCESS\n");
    if (cache.get_last() != dog)
        printf("Get Last Item: FAILED\n");
    else
        printf("Get Last Item: SUCCESS\n");

    if (pDiskItem->get_value() != lion_sound)
        printf("value check: FAILED\n");
    else
        printf("value check: SUCCESS\n");
}