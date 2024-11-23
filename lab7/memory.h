#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>

#define PAGE_SIZE 4096  // Размер страницы в байтах

struct Page {
    void* address;      // Адрес страницы
    bool is_free;       // Флаг, указывающий, свободна ли страница
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
    void free_memory(Process process);
    void write_memory(void* address, const void* data, size_t size);
    void read_memory(void* address, void* data, size_t size);

private:
    std::unordered_map<int, std::vector<Page>> pagesForProcess;
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