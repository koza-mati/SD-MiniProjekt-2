#pragma once
#include <cstddef>

/**
 * Szablonowa tablica dynamiczna z automatycznym rozszerzaniem (podwajanie pojemności).
 * Implementacja własna - bez użycia STL.
 * Złożoności:
 *  - pushBack: O(1) amortyzowane
 *  - popBack:  O(1)
 *  - operator[]/getAt: O(1)
 *  - clear: O(1) (bez dealokacji) lub O(n) (z dealokacją)
 */
template <typename T>
class DynamicArray {
private:
    T* data_;                  // bufor z elementami
    std::size_t size_;         // aktualna liczba elementów
    std::size_t capacity_;     // pojemność bufora

    void grow() {
        std::size_t newCap = (capacity_ == 0) ? 1 : capacity_ * 2;
        T* newData = new T[newCap];
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = data_[i];
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCap;
    }

public:
    DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

    ~DynamicArray() { delete[] data_; }

    // Brak kopiowania - kopiec nie potrzebuje, a ułatwia uniknięcie błędów.
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    void pushBack(const T& element) {
        if (size_ >= capacity_) {
            grow();
        }
        data_[size_++] = element;
    }

    void popBack() {
        if (size_ > 0) {
            --size_;
        }
    }

    T& operator[](std::size_t index) { return data_[index]; }
    const T& operator[](std::size_t index) const { return data_[index]; }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    void clear() noexcept { size_ = 0; }   // bez realokacji - szybciej
};
