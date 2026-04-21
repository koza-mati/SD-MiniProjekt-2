#pragma once

#include <fstream>
#include <random>
#include <sstream>

template <typename T, typename PriorityType>
LinkedListPriorityQueue<T, PriorityType>::~LinkedListPriorityQueue() {
    // Destruktor zwalnia cala dynamicznie zaalokowana liste.
    clear();
}

template <typename T, typename PriorityType>
void LinkedListPriorityQueue<T, PriorityType>::clear() {
    // Iteracyjnie usuwamy wszystkie wezly, aby nie pozostawic wyciekow pamieci.
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
    // Wstawienie na poczatek jest stale, maksimum wyszukujemy dopiero przy odczycie/usunieciu.
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
    // Lista nie jest uporzadkowana, wiec przed usunieciem szukamy maksimum liniowo.
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

    // Poniewaz lista nie jest posortowana, maksimum trzeba wyszukac liniowo.
    return findMaxNode(nullptr)->entry;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::modifyKey(const T& value, const PriorityType& newPriority) {
    // Szukamy pierwszego wezla o podanej wartosci i zmieniamy tylko jego priorytet.
    Node* node = findNode(value);
    if (node == nullptr) {
        return false;
    }

    node->entry.priority = newPriority;
    return true;
}

template <typename T, typename PriorityType>
std::size_t LinkedListPriorityQueue<T, PriorityType>::size() const {
    // Rozmiar przechowujemy na biezaco, bez potrzeby przechodzenia po calej liscie.
    return size_;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::empty() const {
    // Pusta kolejka to taka, w ktorej licznik elementow wynosi 0.
    return size_ == 0;
}

template <typename T, typename PriorityType>
bool LinkedListPriorityQueue<T, PriorityType>::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    // Zapisujemy wszystkie wezly w kolejnosci przechodzenia po liscie.
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

    // Najpierw usuwamy stary stan, potem budujemy nowa liste.
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
            // Wczytane rekordy dopisujemy standardowa operacja insert.
            insert(value, priority);
        }
    }

    return true;
}

template <typename T, typename PriorityType>
void LinkedListPriorityQueue<T, PriorityType>::generateRandom(std::size_t count,
                                                              PriorityType minPriority,
                                                              PriorityType maxPriority) {
    // Losowe dane maja zastapic cala dotychczasowa zawartosc kolejki.
    clear();

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<long long> valueDistribution(1, static_cast<long long>(count * 10 + 100));
    std::uniform_int_distribution<long long> priorityDistribution(
        static_cast<long long>(minPriority),
        static_cast<long long>(maxPriority));

    for (std::size_t i = 0; i < count; ++i) {
        // Podobnie jak w kopcu, losujemy wartosc i odpowiadajacy jej priorytet.
        insert(static_cast<T>(valueDistribution(generator)),
               static_cast<PriorityType>(priorityDistribution(generator)));
    }
}

template <typename T, typename PriorityType>
typename LinkedListPriorityQueue<T, PriorityType>::Node*
LinkedListPriorityQueue<T, PriorityType>::findNode(const T& value) const {
    // Przechodzimy po liscie od poczatku do konca az trafimy na szukana wartosc.
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
    // `best` przechowuje aktualnie najlepszy element znaleziony podczas przejscia.
    Node* best = head_;
    Node* bestPrevious = nullptr;
    Node* currentPrevious = nullptr;

    // Przy rownych priorytetach wygrywa mniejsze `order`, czyli element dodany wczesniej.
    for (Node* current = head_; current != nullptr; current = current->next) {
        if (hasHigherPriority(current->entry, best->entry)) {
            best = current;
            bestPrevious = currentPrevious;
        }
        currentPrevious = current;
    }

    if (previous != nullptr) {
        // Dla extractMax potrzebujemy rowniez wskaznika na element poprzedzajacy maksimum.
        *previous = bestPrevious;
    }

    return best;
}
