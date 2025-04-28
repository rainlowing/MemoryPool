#pragma once

#include <stddef.h>
#include <array>
#include <mutex>
#include <sys/mman.h>

class PageCache {
private:
    PageCache() {
        freeList_.fill(nullptr);
    }

    PageCache(const PageCache &) = delete;
    PageCache &operator=(const PageCache &) = delete;

    static const size_t PAGE_SIZE = 4096;
    static const size_t MAX_PAGES = 129;

    struct Span {
        void    *start_address;
        size_t  page_count;
        Span    *next;
    };

    std::array<Span *, MAX_PAGES> freeList_;
    std::mutex mutex_;

public:
    static PageCache& getInstance() {
        static PageCache instance;
        return instance;
    }

    void *allocateSpan(size_t pages);

private:
    void *splitSpan(Span *span, size_t pages);
};