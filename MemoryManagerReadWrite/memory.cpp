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

Page MemoryManager::getPage(int id) {
    return pages[id];
}

void MemoryManager::allocate_memory(Process& process) {
    size_t processSize = strlen(process.content) + sizeof(process.id);
    int processPagesCount = std::ceil((double)processSize / PAGE_SIZE);

    if (freePagesCount >= processPagesCount) {
        int pagesFilled = 0;
        size_t bytesLeft = processSize;
        for (int i = 0; i < pages.size(); i++) {
            auto& page = pages[i];
            if (page.is_free) {
                page.is_free = false;
                size_t bytesToCopy = std::min(bytesLeft, (size_t)PAGE_SIZE);

                // Copy the content
                std::memcpy(page.address, process.content + pagesFilled * PAGE_SIZE, bytesToCopy);
                
                pagesForProcess[process.id].push_back(i);
                pagesFilled++;
                bytesLeft -= bytesToCopy;
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

void MemoryManager::read_memory(void* dst, void* src, size_t size) {
    if (dst == nullptr || src == nullptr || size > PAGE_SIZE) {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
    else {

        std::memcpy(dst, src, size);
    }
}

void MemoryManager::write_memory(void* dst,  void* src, size_t size) {
    if (dst == nullptr || src == nullptr || size > PAGE_SIZE) {
        std::cerr << "Error: Data size exceeds page size." << std::endl;
    }
    else {
        memcpy(dst, src, size);
    }
}

