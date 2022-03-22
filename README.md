To make builds and discussion easier I kept implementation in the header files.

There is a build.sh for macos/linux.
There is a build.bat for windows with gcc and g++ installed on the path.

Normally for build systems in a cross platform manner i would use CMAKE, Bootstrapped by a python script. This script would handle building missing prebuilt binaries before bootstrapping CMAKE. This tends to be my usual modern build method.

## LRUCacheItem/LRUCache Architecture Notes:

Had I had more time, I would have made LRUCache a template class file. It would publish a factory function for it's instance to create a new LRUCacheItem. Keys would need to serialize on return, and data is serialized and deserialized on load from the disk cache.

In a SOLID design pattern, the LRU Cache takes an IDiskCache, there is only one implementation in this file, which is an sqlite back end. But this virtual class allows many other cache storage back ends. Using SQL or SQLITE as a cache backend is a bit silly , becuase sql is already implementing a type of LRU Cache in the backend to make item retrieval as fast as possible :-)

## Replacing SQLITE with a more memory smart backend!

CacheTable (RESERVED MAX_INDEX x DESIRED_RESERVED_SPACE)
byte packed: 0xFF, 0xFF, ID LENGTH, ID LENGTH, ID LENGTH, ID LENGTH, IDENTIFIER, RECORD START, RECORD START, RECORD START, RECORD START, RECORD END, RECORD END, RECORD END, RECORD END

This would allow the application to scan the byte packed record header for it's desired identifier, and then use the record start and record end to seek and deserialize from a flat file.
The Identifiers could also be loaded up into a tree structure for even faster lookup in memory, or even a B+Tree on disk.

Without the need to serialize/deserialize to a string for SQLite as well, you could also bytepack your objects in the file to save disk space.

## CONCURRENCY

This code is not Concurrent Safe, as the collection owns the item, you must do your work on it and save it. This allows the is_dirty flag to work.
To make this concurrent save, the getters would have to pull the item and return a copy which you own, that you can later put back in, probably with a flag to re-cache or go straight to disk.
Currently the collection owns and delets all items, so using this code concurrently can lead to access violations.

## NOTE

I did not test this on Windows, but it should work with cygwin gcc/g++ and not much tweaking otherwise ;-)
