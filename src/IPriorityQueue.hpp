#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "PriorityQueueCommon.hpp"

template <typename T, typename PriorityType = int>
class IPriorityQueue {
public:
    using Entry = PriorityQueueEntry<T, PriorityType>;

    virtual ~IPriorityQueue() = default;

    virtual void clear() = 0;
    virtual void insert(const T& value, const PriorityType& priority) = 0;
    virtual std::optional<Entry> extractMax() = 0;
    virtual std::optional<Entry> peek() const = 0;
    virtual bool modifyKey(const T& value, const PriorityType& newPriority) = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual bool saveToCSV(const std::string& filename) const = 0;
    virtual bool loadFromCSV(const std::string& filename) = 0;
    virtual void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) = 0;
};
