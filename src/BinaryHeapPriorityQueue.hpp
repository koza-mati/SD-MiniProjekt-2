#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "IPriorityQueue.hpp"

/**
 * Klasa BinaryHeapPriorityQueue implementuje kolejkę priorytetową opartą o kopiec binarny (max-heap).
 * Kopiec przechowywany jest w tablicy zgodnie z reprezentacją: parent = (i-1)/2, left = 2i+1, right = 2i+2.
 * Przy równych priorytetach FIFO zapewnia licznik `nextOrder_` (mniejszy order = wcześniej dodany).
 * Złożoności czasowe:
 * - insert:      O(log n) (heapifyUp)
 * - extractMax:  O(log n) (heapifyDown)
 * - peek:        O(1)
 * - modifyKey:   O(n) (liniowe wyszukiwanie + O(log n) na naprawę kopca)
 * - size/empty:  O(1)
 * - clear:       O(n)
 */
template <typename T, typename PriorityType = int>
class BinaryHeapPriorityQueue final : public IPriorityQueue<T, PriorityType> {
public:
    using Entry = typename IPriorityQueue<T, PriorityType>::Entry;

    BinaryHeapPriorityQueue() = default;                                             // konstruktor - O(1)

    void clear() override;                                                           // czyszczenie kopca - O(n)
    void insert(const T& value, const PriorityType& priority) override;              // wstawienie elementu - O(log n)
    std::optional<Entry> extractMax() override;                                      // pobranie i usunięcie max - O(log n)
    std::optional<Entry> peek() const override;                                      // podgląd maksimum bez usuwania - O(1)
    bool modifyKey(const T& value, const PriorityType& newPriority) override;        // zmiana priorytetu elementu - O(n)
    std::size_t size() const override;                                               // liczba elementów - O(1)
    bool empty() const override;                                                     // czy kopiec pusty - O(1)
    bool saveToCSV(const std::string& filename) const override;                      // zapis do pliku CSV - O(n)
    bool loadFromCSV(const std::string& filename) override;                          // wczytanie z pliku CSV - O(n log n)
    void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) override; // losowe wypełnienie - O(n log n)

private:
    // Kopiec przechowywany jest w tablicy zgodnie z klasyczną reprezentacją:
    // parent = (i - 1) / 2, left = 2*i + 1, right = 2*i + 2.
    std::vector<Entry> data_;       // bufor elementów kopca w układzie tablicowym
    // Licznik kolejności dodawania potrzebny do FIFO przy równych priorytetach.
    std::size_t nextOrder_ = 0;     // monotonicznie rosnący licznik dla porządku FIFO

    void heapifyUp(std::size_t index);     // przywrócenie własności kopca w górę - O(log n)
    void heapifyDown(std::size_t index);   // przywrócenie własności kopca w dół - O(log n)
    std::size_t findIndexByValue(const T& value) const; // liniowe wyszukiwanie indeksu po wartości - O(n)
};

#include "BinaryHeapPriorityQueue.tpp"
