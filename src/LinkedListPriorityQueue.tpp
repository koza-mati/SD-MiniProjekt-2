#pragma once

#include <fstream>
#include <random>
#include <sstream>

template <typename T, typename PriorityType>
LinkedListPriorityQueue<T, PriorityType>::~LinkedListPriorityQueue() {
    // Destruktor zwalnia całą dynamicznie zaalokowaną listę.
    clear();
}

template <typename T, typename PriorityType>
void LinkedListPriorityQueue<T, PriorityType>::clear() {
    // Iteracyjnie usuwamy wszystkie węzły, aby nie pozostawić wycieków pamięci.
    while (head_ != nullptr) {
        Node* next = head_->next;
        delete head_;
        head_ = next;
    }

    size_ = 0;
    nextOrder_ = 0;
}

template <typename T, typename PriorityType>
void LinkedListPriorityQueue<T, PriorityType>::insert(const T& value, const PriorityType& priority) {
    // Wstawienie na początek jest stałe, maksimum wyszukujemy dopiero przy odczycie/usunięciu.
    Node* node = new Node{{value, priority, nextOrder_++}, head_};
    head_ = node;
    ++size_;
}

template <typename T, typename PriorityType>
std::optional<typename LinkedListPriorityQueue<T, PriorityType>::Entry>
LinkedListPriorityQueue<T, PriorityType>::extractMax() {
    if (head_ == nullptr) {
        return std::nullopt;
    }

    Node* previous = nullptr;
    // Lista nie jest uporządkowana, więc przed usunięciem szukamy maksimum liniowo.
    Node* maxNode = findMaxNode(&previous);
    Entry result = maxNode->entry;

    if (previous == nullptr) {
        head_ = maxNode->next;
    } else {
        previous->next = maxNode->next;
    }

    delete maxNode;
    --size_;
    return result;
}

template <typename T, typename PriorityType>
std::optional<typename LinkedListPriorityQueue<T, PriorityType>::Entry>
LinkedListPriorityQueue<T, PriorityType>::peek() const {
    if (head_ == nullptr) {
        return std::nullopt;
    }

    // Ponieważ lista nie jest posortowana, maksimum trzeba wyszukać liniowo.
    return findMaxNode(nullptr)->entry;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::modifyKey(const T& value, const PriorityType& newPriority) {
    // Szukamy pierwszego węzła o podanej wartości i zmieniamy tylko jego priorytet.
    Node* node = findNode(value);
    if (node == nullptr) {
        return false;
    }

    node->entry.priority = newPriority;
    return true;
}

template <typename T, typename PriorityType>
std::size_t LinkedListPriorityQueue<T, PriorityType>::size() const {
    // Rozmiar przechowujemy na bieżąco, bez potrzeby przechodzenia po całej liście.
    return size_;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::empty() const {
    // Pusta kolejka to taka, w której licznik elementów wynosi 0.
    return size_ == 0;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    // Zapisujemy wszystkie węzły w kolejności przechodzenia po liście.
    file << "value,priority,order\n";
    for (Node* current = head_; current != nullptr; current = current->next) {
        file << current->entry.value << ',' << current->entry.priority << ',' << current->entry.order << '\n';
    }

    return true;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Najpierw usuwamy stary stan, potem budujemy nową listę.
    clear();

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string valueToken;
        std::string priorityToken;

        if (!std::getline(ss, valueToken, ',') || !std::getline(ss, priorityToken, ',')) {
            continue;
        }

        std::stringstream valueStream(valueToken);
        std::stringstream priorityStream(priorityToken);
        T value{};
        PriorityType priority{};

        valueStream >> value;
        priorityStream >> priority;
        if (!valueStream.fail() && !priorityStream.fail()) {
            // Wczytane rekordy dopisujemy standardową operacją insert.
            insert(value, priority);
        }
    }

    return true;
}

template <typename T, typename PriorityType>
void LinkedListPriorityQueue<T, PriorityType>::generateRandom(std::size_t count,
                                                              PriorityType minPriority,
                                                              PriorityType maxPriority) {
    // Losowe dane mają zastąpić całą dotychczasową zawartość kolejki.
    clear();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<long long> valueDistribution(1, static_cast<long long>(count * 10 + 100));
    std::uniform_int_distribution<long long> priorityDistribution(
        static_cast<long long>(minPriority),
        static_cast<long long>(maxPriority));

    for (std::size_t i = 0; i < count; ++i) {
        // Podobnie jak w kopcu, losujemy wartość i odpowiadający jej priorytet.
        insert(static_cast<T>(valueDistribution(generator)),
               static_cast<PriorityType>(priorityDistribution(generator)));
    }
}

template <typename T, typename PriorityType>
typename LinkedListPriorityQueue<T, PriorityType>::Node*
LinkedListPriorityQueue<T, PriorityType>::findNode(const T& value) const {
    // Przechodzimy po liście od początku do końca aż trafimy na szukaną wartość.
    for (Node* current = head_; current != nullptr; current = current->next) {
        if (current->entry.value == value) {
            return current;
        }
    }

    return nullptr;
}

template <typename T, typename PriorityType>
typename LinkedListPriorityQueue<T, PriorityType>::Node*
LinkedListPriorityQueue<T, PriorityType>::findMaxNode(Node** previous) const {
    // `best` przechowuje aktualnie najlepszy element znaleziony podczas przejścia.
    Node* best = head_;
    Node* bestPrevious = nullptr;
    Node* currentPrevious = nullptr;

    // Przy równych priorytetach wygrywa mniejsze `order`, czyli element dodany wcześniej.
    for (Node* current = head_; current != nullptr; current = current->next) {
        if (hasHigherPriority(current->entry, best->entry)) {
            best = current;
            bestPrevious = currentPrevious;
        }
        currentPrevious = current;
    }

    if (previous != nullptr) {
        // Dla extractMax potrzebujemy również wskaźnika na element poprzedzający maksimum.
        *previous = bestPrevious;
    }

    return best;
}
