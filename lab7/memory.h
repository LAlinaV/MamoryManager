#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>

#define PAGE_SIZE 4096  // Размер страницы в байтах

struct Page {
    void* address;      // Адрес страницы
    bool is_free;       // Флаг, указывающий, свободна ли страница
};

class MemoryManager {
public:
    MemoryManager(size_t total_pages);
    ~MemoryManager();
    void* allocate_memory();
    void free_memory(void* address);
    void write_memory(void* address, const void* data, size_t size);
    void read_memory(void* address, void* data, size_t size);

private:
    std::vector<Page> pages;
};

// Функция для измерения времени выполнения
template <typename Func>
double measure_time(Func func);

#endif // MEMORY_MANAGER_H