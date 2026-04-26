#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "IPriorityQueue.hpp"

/**
 * Klasa LinkedListPriorityQueue implementuje kolejke priorytetowa opartA o nieuporzadkowana liste jednokierunkowa.
 * Lista jest nieposortowana - wstawienie jest tanie, ale wyszukanie maksimum wymaga przejscia calej listy.
 * Przy rownych priorytetach FIFO zapewnia licznik `nextOrder_` (mniejszy order = wczesniej dodany).
 * Zlozonosci czasowe:
 * - insert:      O(1) (dodanie na poczatek listy)
 * - extractMax:  O(n) (przeszukanie listy w poszukiwaniu maksimum)
 * - peek:        O(n) (jak wyzej, bez usuwania)
 * - modifyKey:   O(n) (liniowe wyszukiwanie wezla)
 * - size/empty:  O(1)
 * - clear:       O(n)
 */
template <typename T, typename PriorityType = int>
class LinkedListPriorityQueue final : public IPriorityQueue<T, PriorityType> {
public:
    using Entry = typename IPriorityQueue<T, PriorityType>::Entry;

    LinkedListPriorityQueue() = default;                                              // konstruktor - O(1)
    ~LinkedListPriorityQueue() override;                                              // destruktor zwalniajacy wezly - O(n)

    LinkedListPriorityQueue(const LinkedListPriorityQueue&) = delete;                 // brak kopiowania (recznie zarzadzana pamiec)
    LinkedListPriorityQueue& operator=(const LinkedListPriorityQueue&) = delete;      // brak przypisania kopiujacego

    void clear() override;                                                            // zwolnienie wszystkich wezlow - O(n)
    void insert(const T& value, const PriorityType& priority) override;               // wstawienie na poczatek listy - O(1)
    std::optional<Entry> extractMax() override;                                       // pobranie i usuniecie max - O(n)
    std::optional<Entry> peek() const override;                                       // podglad maksimum bez usuwania - O(n)
    bool modifyKey(const T& value, const PriorityType& newPriority) override;         // zmiana priorytetu elementu - O(n)
    std::size_t size() const override;                                                // liczba elementow - O(1)
    bool empty() const override;                                                      // czy lista pusta - O(1)
    bool saveToCSV(const std::string& filename) const override;                       // zapis do pliku CSV - O(n)
    bool loadFromCSV(const std::string& filename) override;                           // wczytanie z pliku CSV - O(n)
    void generateRandom(std::size_t count, PriorityType minPriority, PriorityType maxPriority) override; // losowe wypelnienie - O(n)

private:
    struct Node {
        // Kazdy wezel listy przechowuje jeden wpis kolejki i wskaznik na nastepny element.
        Entry entry;             // wartosc + priorytet + order (FIFO)
        Node* next = nullptr;    // wskaznik na kolejny wezel lub nullptr na koncu listy
    };

    // Lista jest nieuporzadkowana, dzieki czemu wstawienie jest bardzo proste.
    Node* head_ = nullptr;          // wskaznik na pierwszy wezel listy
    std::size_t size_ = 0;          // aktualna liczba wezlow w liscie
    std::size_t nextOrder_ = 0;     // monotonicznie rosnacy licznik dla porzadku FIFO

    Node* findNode(const T& value) const;                  // wyszukiwanie wezla po wartosci - O(n)
    Node* findMaxNode(Node** previous) const;              // wyszukiwanie wezla o najwyzszym priorytecie - O(n)
};

#include "LinkedListPriorityQueue.tpp"
