#pragma once

#include <fstream>
#include <random>
#include <sstream>
#include <utility>

namespace binary_heap_detail {
// Zakres losowanych wartości jest dobierany proporcjonalnie do liczby elementów.
constexpr long long kRandomValueMultiplier = 10;
constexpr long long kRandomValueOffset = 100;
}  // namespace binary_heap_detail

template <typename T, typename PriorityType>
void BinaryHeapPriorityQueue<T, PriorityType>::clear() noexcept {
    // Usuwamy całą zawartość oraz zerujemy licznik kolejności wstawiania.
    data_.clear();
    nextOrder_ = 0;
}

template <typename T, typename PriorityType>
void BinaryHeapPriorityQueue<T, PriorityType>::insert(const T& value, const PriorityType& priority) {
    // Tworzymy nowy wpis na końcu tablicy.
    data_.push_back(Entry{value, priority, nextOrder_++});
    // Po dodaniu element trafia na koniec tablicy i jest przesuwany w górę kopca.
    heapifyUp(data_.size() - 1);
}

template <typename T, typename PriorityType>
std::optional<typename BinaryHeapPriorityQueue<T, PriorityType>::Entry>
BinaryHeapPriorityQueue<T, PriorityType>::extractMax() {
    if (data_.empty()) {
        return std::nullopt;
    }

    // W MAX-kopcu maksimum zawsze znajduje się w korzeniu.
    Entry result = std::move(data_.front());
    // Korzeń zastępujemy ostatnim elementem, a potem odtwarzamy własność kopca.
    data_.front() = std::move(data_.back());
    data_.pop_back();

    if (!data_.empty()) {
        heapifyDown(0);
    }

    return result;
}

template <typename T, typename PriorityType>
std::optional<typename BinaryHeapPriorityQueue<T, PriorityType>::Entry>
BinaryHeapPriorityQueue<T, PriorityType>::peek() const {
    if (data_.empty()) {
        return std::nullopt;
    }

    // Podejrzenie maksimum nie zmienia struktury, zwracamy tylko korzeń.
    return data_.front();
}

template <typename T, typename PriorityType>
bool BinaryHeapPriorityQueue<T, PriorityType>::modifyKey(const T& value, const PriorityType& newPriority) {
    // Szukamy pierwszego elementu o zadanej wartości.
    const std::size_t index = findIndexByValue(value);
    if (index == data_.size()) {
        return false;
    }

    const PriorityType oldPriority = data_[index].priority;
    data_[index].priority = newPriority;

    // Kierunek naprawy zależy od tego, czy priorytet wzrósł czy zmalał.
    if (newPriority > oldPriority) {
        heapifyUp(index);
    } else if (newPriority < oldPriority) {
        heapifyDown(index);
    }

    return true;
}

template <typename T, typename PriorityType>
std::size_t BinaryHeapPriorityQueue<T, PriorityType>::size() const noexcept {
    // Rozmiar to po prostu liczba wpisów w tablicy kopca.
    return data_.size();
}

template <typename T, typename PriorityType>
bool BinaryHeapPriorityQueue<T, PriorityType>::empty() const noexcept {
    // Kolejka jest pusta, gdy tablica nie zawiera żadnych elementów.
    return data_.empty();
}

template <typename T, typename PriorityType>
bool BinaryHeapPriorityQueue<T, PriorityType>::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    // Zapisujemy cały stan wewnętrzny kopca, żeby można było go później odtworzyć.
    file << "value,priority,order\n";
    for (const auto& entry : data_) {
        file << entry.value << ',' << entry.priority << ',' << entry.order << '\n';
    }

    return true;
}

template <typename T, typename PriorityType>
bool BinaryHeapPriorityQueue<T, PriorityType>::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Wczytanie rozpoczynamy od wyczyszczenia starej zawartości.
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
            // Elementy odtwarzamy przez zwykły insert, żeby zachować poprawność kopca.
            insert(value, priority);
        }
    }

    return true;
}

template <typename T, typename PriorityType>
void BinaryHeapPriorityQueue<T, PriorityType>::generateRandom(std::size_t count,
                                                              PriorityType minPriority,
                                                              PriorityType maxPriority) {
    // Generowanie losowe tworzy nową strukturę od zera.
    clear();

    std::random_device rd;
    std::mt19937 generator(rd());
    const long long valueRangeMax =
        static_cast<long long>(count) * binary_heap_detail::kRandomValueMultiplier
        + binary_heap_detail::kRandomValueOffset;
    std::uniform_int_distribution<long long> valueDistribution(1, valueRangeMax);
    std::uniform_int_distribution<long long> priorityDistribution(
        static_cast<long long>(minPriority),
        static_cast<long long>(maxPriority));

    for (std::size_t i = 0; i < count; ++i) {
        // Dla każdego elementu losujemy zarówno wartość, jak i priorytet.
        insert(static_cast<T>(valueDistribution(generator)),
               static_cast<PriorityType>(priorityDistribution(generator)));
    }
}

template <typename T, typename PriorityType>
void BinaryHeapPriorityQueue<T, PriorityType>::heapifyUp(std::size_t index) {
    // Przesuwamy element w górę, dopóki ma wyższy priorytet niż rodzic.
    while (index > 0) {
        const std::size_t parent = (index - 1) / 2;
        if (!hasHigherPriority(data_[index], data_[parent])) {
            break;
        }

        std::swap(data_[index], data_[parent]);
        index = parent;
    }
}

template <typename T, typename PriorityType>
void BinaryHeapPriorityQueue<T, PriorityType>::heapifyDown(std::size_t index) {
    // Przesuwamy element w dół, dopóki którekolwiek dziecko powinno być nad nim.
    while (true) {
        const std::size_t left = index * 2 + 1;
        const std::size_t right = index * 2 + 2;
        std::size_t largest = index;

        // W MAX-kopcu wybieramy dziecko o najwyższym priorytecie.
        if (left < data_.size() && hasHigherPriority(data_[left], data_[largest])) {
            largest = left;
        }

        if (right < data_.size() && hasHigherPriority(data_[right], data_[largest])) {
            largest = right;
        }

        if (largest == index) {
            // Jeżeli rodzic nadal jest najlepszy, własność kopca została przywrócona.
            break;
        }

        std::swap(data_[index], data_[largest]);
        index = largest;
    }
}

template <typename T, typename PriorityType>
std::size_t BinaryHeapPriorityQueue<T, PriorityType>::findIndexByValue(const T& value) const {
    // To wyszukiwanie jest liniowe, bo kopiec porządkuje po priorytecie, a nie po wartości.
    for (std::size_t i = 0; i < data_.size(); ++i) {
        if (data_[i].value == value) {
            return i;
        }
    }

    return data_.size();
}
