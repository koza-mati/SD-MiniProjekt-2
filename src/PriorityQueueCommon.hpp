#pragma once

#include <cstddef>
#include <ostream>

/**
 * Struktura PriorityQueueEntry reprezentuje pojedynczy wpis w kolejce priorytetowej.
 * Pole `order` pozwala zachować porządek FIFO przy równych priorytetach (mniejszy order = wcześniej dodany).
 * Wszystkie implementacje kolejek (kopiec, lista) operują na tej samej strukturze, co ułatwia porównywanie.
 */
template <typename T, typename PriorityType>
struct PriorityQueueEntry {
    // Wartość przechowywana w kolejce priorytetowej.
    T value{};                  // przechowywana wartość (np. int, string)
    // Klucz decydujący o kolejności obsługi elementu.
    PriorityType priority{};    // im większa wartość, tym wyższy priorytet (max-heap)
    // `order` pozwala zachować FIFO dla elementów o tym samym priorytecie.
    std::size_t order{};        // numer kolejności wstawienia
};

/**
 * Funkcja porównująca dwa wpisy kolejki priorytetowej.
 * Zwraca true jeśli `left` ma wyższy priorytet niż `right`.
 * Przy równych priorytetach decyduje porządek FIFO (mniejszy order wygrywa).
 * Złożoność: O(1).
 */
template <typename T, typename PriorityType>
inline bool hasHigherPriority(const PriorityQueueEntry<T, PriorityType>& left,
                              const PriorityQueueEntry<T, PriorityType>& right) {
    // Najpierw porównujemy główny priorytet.
    if (left.priority != right.priority) {
        return left.priority > right.priority;
    }

    // Przy remisie wygrywa element dodany wcześniej.
    return left.order < right.order;
}

/**
 * Operator wypisywania wpisu kolejki w formacie "value (priority)".
 * Używany przy listingu zawartości kolejki w menu.
 * Złożoność: O(1) (przy założeniu staloczasowego << dla T i PriorityType).
 */
template <typename T, typename PriorityType>
inline std::ostream& operator<<(std::ostream& os, const PriorityQueueEntry<T, PriorityType>& entry) {
    os << entry.value << " (" << entry.priority << ")";
    return os;
}
