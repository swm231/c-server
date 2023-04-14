#ifndef _FREE_LIST_H_
#define _FREE_LIST_H_

#include "Common.h"

class CentralCache{
private:
    static SpanList* spanlist_;
public:
    static void* CentralAllocate(size_t size);
};

class FreeList{
private:
    void* head_;
    size_t size_, max_size_;
    std::atomic_flag flag_;
public:
    FreeList(int _size) : 
        head_(nullptr), size_(0), max_size_(1 << (_size + 3)) {}
    ~FreeList() {}

    void Push(void* obj)
    {
        lock();
        NxtObj(obj) = head_;
        head_ = obj;
        ++ size_;
        // printf("%ld字节回收完毕,还剩%ld块,roundup:%ld\n", max_size_, size_, Size::Roundup(size_));
        unlock();
    }

    void PushRange(void* start, void* end, size_t n)
    {
        if(end != nullptr)
            NxtObj(end) = head_;
        head_ = start;
        size_ += n;
    }

    void* Get()
    {
        lock();
        if(!size_) {
            PushRange(CentralCache::CentralAllocate(max_size_),
            nullptr, 8 * 1024 / max_size_);
        }
        void* obj = head_;
        head_ = NxtObj(obj);
        -- size_;
        // printf("%ld字节分配完毕,还剩%ld块,roundup:%ld\n", max_size_, size_, Size::Roundup(size_));
        unlock();
        return obj;
    }

    void *GetAll()
    {
        lock();
        void* obj = head_;
        head_ = nullptr;
        size_ = 0;
        unlock();
        return obj;
    }

    bool Empty()
    {
        return size_ == 0;
    }

    size_t Size()
    {
        return size_;
    }

    void lock(){
        while(flag_.test_and_set());
    }

    void unlock(){
        flag_.clear();
    }
};

#endif