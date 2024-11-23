#include "memory.h"

MemoryManager::MemoryManager(size_t total_pages) {
    pages.reserve(total_pages);
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

void* MemoryManager::allocate_memory() {
    for (auto& page : pages) {
        if (page.is_free) {
            page.is_free = false;
            return page.address;
        }
    }
    return nullptr;  // Если свободных страниц нет
}

void MemoryManager::free_memory(void* address) {
    for (auto& page : pages) {
        if (page.address == address) {
            page.is_free = true;
            return;
        }
    }
}

void MemoryManager::write_memory(void* address, const void* data, size_t size) {
    if (size <= PAGE_SIZE) {
        memcpy(address, data, size);
    }
    else {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
}

void MemoryManager::read_memory(void* address, void* data, size_t size) {
    if (size <= PAGE_SIZE) {
        memcpy(data, address, size);
    }
    else {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
}
