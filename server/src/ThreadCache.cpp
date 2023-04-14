#include "ThreadCache.h"

FreeList* ThreadCache::freelist_[NLISTS] = {
    new FreeList(0),
    new FreeList(1),
    new FreeList(2),
    new FreeList(3),
    new FreeList(4),
    new FreeList(5),
    new FreeList(6),
    new FreeList(7),
    new FreeList(8),
    new FreeList(9)
};

SpanList* CentralCache::spanlist_ = new SpanList();

void* ThreadCache::Allocate(size_t size)
{
    size_t index = Size::Index(size);
    return freelist_[index]->Get();
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
    size_t index = Size::Index(size);
    freelist_[index]->Push(ptr);
    }

void* CentralCache::CentralAllocate(size_t size)
{
    if(spanlist_->Empty())
        for(int i = 0; i < 5; ++i)
            spanlist_->AddSpan();

    Span* obj = spanlist_->GetSpan();

    char* cur = (char*)obj->ptr_;
    char* rtn = (char*)obj->ptr_;
    for(size_t i = size; i < 8 * 1024; i += size){
        NxtObj(cur) = cur + size;
        cur += size;
    }
    NxtObj(cur) = nullptr;
    return (void*)rtn;
}