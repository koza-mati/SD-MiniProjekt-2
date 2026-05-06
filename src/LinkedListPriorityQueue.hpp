#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "IPriorityQueue.hpp"

/**
 * Klasa LinkedListPriorityQueue implementuje kolejkę priorytetową opartą o nieuporządkowaną listę jednokierunkową.
 * Lista jest nieposortowana - wstawienie jest tanie, ale wyszukanie maksimum wymaga przejścia całej listy.
 * Przy równych priorytetach FIFO zapewnia licznik `nextOrder_` (mniejszy order = wcześniej dodany).
 * Złożoności czasowe:
 * - insert:      O(1) (dodanie na początek listy)
 * - extractMax:  O(n) (przeszukanie listy w poszukiwaniu maksimum)
 * - peek:        O(n) (jak wyżej, bez usuwania)
 * - modifyKey:   O(n) (liniowe wyszukiwanie węzła)
 * - size/empty:  O(1)
 * - clear:       O(n)
 */
template <typename T, typename PriorityType = int>
class LinkedListPriorityQueue final : public IPriorityQueue<T, PriorityType> {
public:
    using Entry = typename IPriorityQueue<T, PriorityType>::Entry;

    LinkedListPriorityQueue() = default;                                              // konstruktor - O(1)
    ~LinkedListPriorityQueue() override;                                              // destruktor zwalniający węzły - O(n)

    LinkedListPriorityQueue(const LinkedListPriorityQueue&) = delete;                 // brak kopiowania (ręcznie zarządzana pamięć)
    LinkedListPriorityQueue& operator=(const LinkedListPriorityQueue&) = delete;      // brak przypisania kopiującego

    void clear() noexcept override;                                                   // zwolnienie wszystkich węzłów - O(n)
    void insert(const T& value, const PriorityType& priority) override;               // wstawienie na początek listy - O(1)
    std::optional<Entry> extractMax() override;                                       // pobranie i usunięcie max - O(n)
    std::optional<Entry> peek() const override;                                       // podgląd maksimum bez usuwania - O(n)
    bool modifyKey(const T& value, const PriorityType& newPriority) override;         // zmiana priorytetu elementu - O(n)
    std::size_t size() const noexcept override;                                       // liczba elementów - O(1)
    bool empty() const noexcept override;                                             // czy lista pusta - O(1)
    bool saveToCSV(const std::string& filename) const override;                       // zapis do pliku CSV - O(n)
    bool loadFromCSV(const std::string& filename) override;                           // wczytanie z pliku CSV - O(n)
    void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) override; // losowe wypełnienie - O(n)

private:
    struct Node {
        // Każdy węzeł listy przechowuje jeden wpis kolejki i wskaźnik na następny element.
        Entry entry;             // wartość + priorytet + order (FIFO)
        Node* next = nullptr;    // wskaźnik na kolejny węzeł lub nullptr na końcu listy
    };

    // Lista jest nieuporządkowana, dzięki czemu wstawienie jest bardzo proste.
    Node* head_ = nullptr;          // wskaźnik na pierwszy węzeł listy
    std::size_t size_ = 0;          // aktualna liczba węzłów w liście
    std::size_t nextOrder_ = 0;     // monotonicznie rosnący licznik dla porządku FIFO

    Node* findNode(const T& value) const;                  // wyszukiwanie węzła po wartości - O(n)
    Node* findMaxNode(Node** previous) const;              // wyszukiwanie węzła o najwyższym priorytecie - O(n)
};

#include "LinkedListPriorityQueue.tpp"
