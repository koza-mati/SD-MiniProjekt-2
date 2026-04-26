#pragma once

#include <cstddef>
#include <ostream>

/**
 * Struktura PriorityQueueEntry reprezentuje pojedynczy wpis w kolejce priorytetowej.
 * Pole `order` pozwala zachowac porzadek FIFO przy rownych priorytetach (mniejszy order = wczesniej dodany).
 * Wszystkie implementacje kolejek (kopiec, lista) operuja na tej samej strukturze, co ulatwia porownywanie.
 */
template <typename T, typename PriorityType>
struct PriorityQueueEntry {
    // Wartosc przechowywana w kolejce priorytetowej.
    T value{};                  // przechowywana wartosc (np. int, string)
    // Klucz decydujacy o kolejnosci obslugi elementu.
    PriorityType priority{};    // im wieksza wartosc, tym wyzszy priorytet (max-heap)
    // `order` pozwala zachowac FIFO dla elementow o tym samym priorytecie.
    std::size_t order{};        // numer kolejnosci wstawienia
};

/**
 * Funkcja porownujaca dwa wpisy kolejki priorytetowej.
 * Zwraca true jesli `left` ma wyzszy priorytet niz `right`.
 * Przy rownych priorytetach decyduje porzadek FIFO (mniejszy order wygrywa).
 * Zlozonosc: O(1).
 */
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

/**
 * Operator wypisywania wpisu kolejki w formacie "value (priority)".
 * Uzywany przy listingu zawartosci kolejki w menu.
 * Zlozonosc: O(1) (przy zalozeniu staloczasowego << dla T i PriorityType).
 */
template <typename T, typename PriorityType>
inline std::ostream& operator<<(std::ostream& os, const PriorityQueueEntry<T, PriorityType>& entry) {
    os << entry.value << " (" << entry.priority << ")";
    return os;
}
