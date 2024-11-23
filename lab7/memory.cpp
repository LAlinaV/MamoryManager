#include "memory.h"

MemoryManager::MemoryManager(size_t total_pages) {
    pages.reserve(total_pages);
    freePagesCount = total_pages;
    for (size_t i = 0; i < total_pages; i++) {
        Page new_page;
        new_page.address = malloc(PAGE_SIZE);
        new_page.is_free = true;
        pages.push_back(new_page);
    }
}

MemoryManager::~MemoryManager() {
    for (auto& page : pages) {
        free(page.address);
    }
}

void MemoryManager::allocate_memory(Process process) {
    size_t processSize = sizeof(process);
    int processPagesCount = std::ceil((double)processSize / PAGE_SIZE);

    if (freePagesCount >= processPagesCount) {
        int pagesFilled = 0;
        for (auto& page : pages) {
            if (page.is_free) {
                page.is_free = false;
                std::memcpy(page.address, &process + pagesFilled * PAGE_SIZE, PAGE_SIZE);
                pagesForProcess[process.id].push_back(page);
                pagesFilled++;
                freePagesCount--;
            }
        }
    }
}

void MemoryManager::free_memory(Process process) {
    std::vector<Page> processPages = pagesForProcess[process.id];
    for (auto& page : processPages) {
        free(page.address);
        page.is_free = true;
        freePagesCount++;
    }
}

void MemoryManager::read_memory(void* address, void* data, size_t size) {
    if (address == nullptr || data == nullptr || size > PAGE_SIZE) {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
    else {
        memcpy(data, address, size);
    }
}

void MemoryManager::write_memory(void* address, const void* data, size_t size) {
    if (address == nullptr || data == nullptr || size > PAGE_SIZE) {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
    else {
        memcpy(address, data, size);
    }
}

