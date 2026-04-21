#pragma once

#include <cstddef>
#include <ostream>

template <typename T, typename PriorityType>
struct PriorityQueueEntry {
    // Wartosc przechowywana w kolejce priorytetowej.
    T value{};
    // Klucz decydujacy o kolejnosci obslugi elementu.
    PriorityType priority{};
    // `order` pozwala zachowac FIFO dla elementow o tym samym priorytecie.
    std::size_t order{};
};

template <typename T, typename PriorityType>
inline bool hasHigherPriority(const PriorityQueueEntry<T, PriorityType>& left,
                              const PriorityQueueEntry<T, PriorityType>& right) {
    // Najpierw porownujemy glowny priorytet.
    if (left.priority != right.priority) {
        return left.priority > right.priority;
    }

    // Przy remisie wygrywa element dodany wczesniej.
    return left.order < right.order;
}

template <typename T, typename PriorityType>
inline std::ostream& operator<<(std::ostream& os, const PriorityQueueEntry<T, PriorityType>& entry) {
    os << entry.value << " (" << entry.priority << ")";
    return os;
}
