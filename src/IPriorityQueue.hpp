#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "PriorityQueueCommon.hpp"

/**
 * Interfejs IPriorityQueue definiuje kontrakt dla wszystkich implementacji kolejki priorytetowej
 * (np. BinaryHeapPriorityQueue, LinkedListPriorityQueue). Dzięki niemu menu i benchmarki mogą
 * pracować na dowolnej strukturze przez wskaźnik/referencję do interfejsu.
 * Złożoności czasowe podanych operacji zależą od konkretnej implementacji - patrz odpowiednie klasy.
 */
template <typename T, typename PriorityType = int>
class IPriorityQueue {
public:
    using Entry = PriorityQueueEntry<T, PriorityType>;

    virtual ~IPriorityQueue() = default;                                                       // wirtualny destruktor

    virtual void clear() = 0;                                                                  // wyczyszczenie kolejki
    virtual void insert(const T& value, const PriorityType& priority) = 0;                     // wstawienie elementu z priorytetem
    virtual std::optional<Entry> extractMax() = 0;                                             // pobranie i usunięcie elementu o najwyższym priorytecie
    virtual std::optional<Entry> peek() const = 0;                                             // podgląd elementu o najwyższym priorytecie bez usuwania
    virtual bool modifyKey(const T& value, const PriorityType& newPriority) = 0;               // zmiana priorytetu wskazanego elementu
    virtual std::size_t size() const = 0;                                                      // liczba elementów w kolejce
    virtual bool empty() const = 0;                                                            // czy kolejka jest pusta
    virtual bool saveToCSV(const std::string& filename) const = 0;                             // zapis stanu kolejki do pliku CSV
    virtual bool loadFromCSV(const std::string& filename) = 0;                                 // wczytanie stanu kolejki z pliku CSV
    virtual void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) = 0; // wypełnienie losowymi danymi
};
