#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <queue>

#define PAGE_SIZE 4096  

struct Page {
    void* address;      
    bool is_free;       
};

struct Process {
    int id;
    std::string content;
};

class MemoryManager {
public:
    MemoryManager(size_t total_pages);
    ~MemoryManager();
    void allocate_memory(Process process);
    std::vector<int> free_memory();
    void write_memory(void* address, const void* data, size_t size);
    void read_memory(void* address, void* data, size_t size);
    std::unordered_map<int, std::vector<int>> getPages();

private:
    std::queue<Process> q;
    std::unordered_map<int, std::vector<int>> pagesForProcess;
    std::vector<Page> pages;
    int freePagesCount;
};

template <typename Func>
double measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

#endif // MEMORY_MANAGER_H
