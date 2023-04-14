#ifndef _Thread_Cache_H_
#define _Thread_Cache_H_

#include "Common.h"
#include "FreeList.h"
#include <memory>

class FreeList;
class ThreadCache{
private:
    // 8, 16, 32, 64, 128, 256, 512, 1k, 4k, 8k
    static FreeList* freelist_[NLISTS];
public:
    static void* Allocate(size_t);
    static void Deallocate(void*, size_t);
    
    void* operator new (size_t size){
        return Allocate(size);
    }

    void operator delete(void* ptr, size_t size){
        Deallocate(ptr, size);
    }
};


#endif