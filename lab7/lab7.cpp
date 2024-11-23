#include <iostream>
#include "memory.h"

int main()
{
    const size_t total_pages = 1000;
    MemoryManager manager(total_pages);

    // Эксперимент 1: Время выделения памяти
    std::vector<double> allocation_times;
    for (size_t i = 0; i < total_pages; i++) {
        double time = measure_time([&]() { manager.allocate_memory(); });
        allocation_times.push_back(time);
    }

    // Эксперимент 2: Время освобождения памяти
    std::vector<double> deallocation_times;
    for (size_t i = 0; i < total_pages; i++) {
        void* address = manager.allocate_memory();
        double time = measure_time([&]() { manager.free_memory(address); });
        deallocation_times.push_back(time);
    }

    // Эксперимент 3: Время чтения и записи данных
    std::vector<double> read_write_times;
    for (size_t size = 1; size <= PAGE_SIZE; size *= 2) {
        void* address = manager.allocate_memory();
        char data[PAGE_SIZE];
        memset(data, 'a', size);

        double write_time = measure_time([&]() { manager.write_memory(address, data, size); });
        double read_time = measure_time([&]() { manager.read_memory(address, data, size); });

        read_write_times.push_back(write_time + read_time);
        manager.free_memory(address);
    }

    // Вывод результатов
    std::cout << "Allocation Times:" << std::endl;
    for (const auto& time : allocation_times) {
        std::cout << time << " ";
    }
    std::cout << std::endl;

    std::cout << "Deallocation Times:" << std::endl;
    for (const auto& time : deallocation_times) {
        std::cout << time << " ";
    }
    std::cout << std::endl;

    std::cout << "Read/Write Times:" << std::endl;
    for (const auto& time : read_write_times) {
        std::cout << time << " ";
    }
    std::cout << std::endl;

    return 0;
}