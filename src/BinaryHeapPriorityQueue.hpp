#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "IPriorityQueue.hpp"

template <typename T, typename PriorityType = int>
class BinaryHeapPriorityQueue final : public IPriorityQueue<T, PriorityType> {
public:
    using Entry = typename IPriorityQueue<T, PriorityType>::Entry;

    BinaryHeapPriorityQueue() = default;

    void clear() override;
    void insert(const T& value, const PriorityType& priority) override;
    std::optional<Entry> extractMax() override;
    std::optional<Entry> peek() const override;
    bool modifyKey(const T& value, const PriorityType& newPriority) override;
    std::size_t size() const override;
    bool empty() const override;
    bool saveToCSV(const std::string& filename) const override;
    bool loadFromCSV(const std::string& filename) override;
    void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) override;

private:
    // Kopiec przechowywany jest w tablicy zgodnie z klasyczna reprezentacja:
    // parent = (i - 1) / 2, left = 2*i + 1, right = 2*i + 2.
    std::vector<Entry> data_;
    // Licznik kolejnosci dodawania potrzebny do FIFO przy rownych priorytetach.
    std::size_t nextOrder_ = 0;

    void heapifyUp(std::size_t index);
    void heapifyDown(std::size_t index);
    std::size_t findIndexByValue(const T& value) const;
};

#include "BinaryHeapPriorityQueue.tpp"
