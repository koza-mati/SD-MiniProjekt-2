#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "IPriorityQueue.hpp"

template <typename T, typename PriorityType = int>
class LinkedListPriorityQueue final : public IPriorityQueue<T, PriorityType> {
public:
    using Entry = typename IPriorityQueue<T, PriorityType>::Entry;

    LinkedListPriorityQueue() = default;
    ~LinkedListPriorityQueue() override;

    LinkedListPriorityQueue(const LinkedListPriorityQueue&) = delete;
    LinkedListPriorityQueue& operator=(const LinkedListPriorityQueue&) = delete;

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
    struct Node {
        // Kazdy wezel listy przechowuje jeden wpis kolejki i wskaznik na nastepny element.
        Entry entry;
        Node* next = nullptr;
    };

    // Lista jest nieuporzadkowana, dzieki czemu wstawienie jest bardzo proste.
    Node* head_ = nullptr;
    std::size_t size_ = 0;
    std::size_t nextOrder_ = 0;

    Node* findNode(const T& value) const;
    Node* findMaxNode(Node** previous) const;
};

#include "LinkedListPriorityQueue.tpp"
