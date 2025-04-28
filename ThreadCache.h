#pragma once

#include <stddef.h>
#include <array>

class ThreadCache {
private:
    ThreadCache() = default;
    void *fetchFromCentralCache(size_t index);
    void returnToCentalCache();
    
public:
    static ThreadCache& getInstance() {
        static thread_local ThreadCache instance;
        return instance;
    }

    void *allocate(size_t size);
    void deallocate();

private:
    std::array<void *, FREE_LIST_SIZE> freeList_;
};