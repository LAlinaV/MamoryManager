#include "memory.h"
#include <cmath>
#include <cstring>

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

std::unordered_map<int, std::vector<int>> MemoryManager::getPages() {
    return pagesForProcess;
}

void MemoryManager::allocate_memory(Process process) {
    size_t processSize = process.content.size() + sizeof(process.id);
    int processPagesCount = std::ceil((double)processSize / PAGE_SIZE);

    if (freePagesCount >= processPagesCount) {
        int pagesFilled = 0;
        size_t offset = 0;
        for (int i = 0; i < pages.size(); i++) {
            auto& page = pages[i];
            if (page.is_free) {
                page.is_free = false;
                size_t bytesToCopy = std::min(processSize - offset, (size_t)PAGE_SIZE);
                if (offset == 0) {
                    // Copy the id first
                    std::memcpy(page.address, &process.id, sizeof(process.id));
                    offset += sizeof(process.id);
                    bytesToCopy -= sizeof(process.id);
                }
                if (bytesToCopy > 0) {
                    // Copy the content
                    std::memcpy(static_cast<char*>(page.address) + offset, process.content.data() + offset, bytesToCopy);
                    offset += bytesToCopy;
                }
                pagesForProcess[process.id].push_back(i);
                pagesFilled++;
                freePagesCount--;
                if (pagesFilled >= processPagesCount) {
                    break;
                }
            }
        }
        
        q.push(process);
    }
}

std::vector<int> MemoryManager::free_memory() {
    Process process = q.front();
    q.pop();
    std::vector<int> processPages = pagesForProcess[process.id];
    for (int i : processPages) {
        pages[i].is_free = true;
        freePagesCount++;
        pagesForProcess.erase(process.id);
    }
    
    return processPages;
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

