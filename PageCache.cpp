#include "PageCache.h"

void *PageCache::allocateSpan(size_t pages) {
    if (pages == 0 || pages >= MAX_PAGES) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (freeList_[pages] != nullptr) {
        Span *ret = freeList_[pages];
        freeList_[pages] = ret->next;
        return ret->start_address;
    }

    for (size_t it = pages + 1; it < MAX_PAGES; ++it) {
        if (freeList_[it] != nullptr) {
            Span *span = freeList_[it];
            freeList_[it] = span->next;
            return splitSpan(span, pages);
        }
    }

    void *sys_mem = mmap(nullptr, (MAX_PAGES - 1) * PAGE_SIZE
    , PROT_WRITE | PROT_READ
    , MAP_PRIVATE | MAP_ANONYMOUS,
    -1, 0);
    if (sys_mem == MAP_FAILED) {
        return nullptr;
    }

    Span *span = new Span;
    span->start_address = sys_mem;
    span->page_count = MAX_PAGES - 1;
    span->next = nullptr;
    return splitSpan(span, pages);
}

void *PageCache::splitSpan(Span *span, size_t pages) {
    void *ret = span->start_address;

    if (span->page_count == pages) {
        return ret;
    }

    // 处理剩余部分的元数据 ※未来可使用对象池优化 new 操作
    Span *remainder = new Span;
    remainder->start_address = static_cast<char *>(ret) + pages * PAGE_SIZE;
    remainder->page_count = span->page_count - pages;
    remainder->next = freeList_[remainder->page_count];
    freeList_[remainder->page_count] = remainder;

    // 修改先前 span 的元数据
    span->page_count = pages;
    span->next = nullptr;

    return ret;
}   