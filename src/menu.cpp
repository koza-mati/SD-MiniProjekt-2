#include "menu.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "BinaryHeapPriorityQueue.hpp"
#include "LinkedListPriorityQueue.hpp"

using HeapQueue = BinaryHeapPriorityQueue<int, int>;
using ListQueue = LinkedListPriorityQueue<int, int>;

namespace menu_utils {
using Clock = std::chrono::high_resolution_clock;
// Każdą operację mierzymy 100 razy i liczymy średnią.
constexpr int kBenchmarkAttempts = 100;

unsigned int benchmarkSeed(int size, int attempt) {
    // Deterministyczny seed pozwala odtwarzać te same dane testowe.
    return 123456u + static_cast<unsigned int>(size) * 7919u + static_cast<unsigned int>(attempt) * 131u;
}

std::vector<int> generateBenchmarkData(int size, int operations, unsigned int seed) {
    // Generujemy jeden bufor liczb, z którego korzysta później benchmark.
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1, 100000000);
    std::vector<int> data;
    data.reserve(size + operations);

    for (int i = 0; i < size + operations; ++i) {
        data.push_back(distribution(generator));
    }

    return data;
}

const std::vector<int>& benchmarkSizes() {
    // Zestaw rozmiarów, dla których będą wykonywane pomiary.
    static const std::vector<int> sizes = {10000, 20000, 40000, 80000, 100000, 160000, 320000, 640000};
    return sizes;
}

void clearConsole() {
    // Czyszczenie ekranu poprawia czytelność pracy z menu w konsoli Windows.
    system("cls");
}

int readInt(const std::string& prompt) {
    int value = 0;

    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }

        // Przy błędnym wejściu kasujemy błąd strumienia i odrzucamy pozostałe znaki.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Niepoprawna wartosc. Sprobuj ponownie.\n";
    }
}

void writeSeedsForSize(const std::string& filename, int size) {
    // Zapis seedów do pliku pomaga udokumentować sposób generowania danych.
    std::ofstream seedFile(filename, std::ios::trunc);
    if (!seedFile.is_open()) {
        std::cerr << "Blad otwarcia pliku seedow: " << filename << '\n';
        return;
    }

    seedFile << "Rozmiar;Proba;Seed\n";
    for (int attempt = 0; attempt < kBenchmarkAttempts; ++attempt) {
        seedFile << size << ';' << attempt << ';' << benchmarkSeed(size, attempt) << '\n';
    }
}

void appendMeasurement(std::ofstream& txtFile,
                       const std::string& structureName,
                       const std::string& operationName,
                       int size,
                       long long averageTimeNs) {
    // Jeden wiersz podsumowujący pojedynczą operację dla danego rozmiaru.
    txtFile << structureName << ';' << operationName << ';' << size << ';' << averageTimeNs << '\n';
}

template <typename Queue>
void saveQueueState(Queue& queue, const std::string& filename) {
    if (!queue.saveToCSV(filename)) {
        std::cout << "Nie udalo sie zapisac pliku: " << filename << '\n';
    }
}

template <typename Queue>
void printEntry(const std::optional<typename Queue::Entry>& entry) {
    if (!entry.has_value()) {
        std::cout << "Struktura jest pusta.\n";
        return;
    }

    std::cout << "Element: " << entry->value << ", priorytet: " << entry->priority << '\n';
}

template <typename Queue>
void fillQueueForBenchmark(Queue& queue, const std::vector<int>& values, int size) {
    // Pierwsza połowa danych staje się wartościami, druga połowa ich priorytetami.
    for (int i = 0; i < size; ++i) {
        queue.insert(values[i], values[size + i]);
    }
}

template <typename Queue>
void benchmarkQueue(const std::string& structureName, const std::string& resultsFile) {
    std::ofstream file(resultsFile, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Blad otwarcia pliku wynikowego: " << resultsFile << '\n';
        return;
    }

    std::ofstream txtFile("pomiary.txt", std::ios::app);
    if (!txtFile.is_open()) {
        std::cerr << "Blad otwarcia pliku pomiary.txt\n";
        return;
    }

    file << "Operation,Size,AverageTime_ns\n";

    for (int size : benchmarkSizes()) {
        std::cout << "Testowanie " << structureName << ", rozmiar: " << size << '\n';

        long long sumInsert = 0;
        long long sumExtractMax = 0;
        long long sumPeek = 0;
        long long sumModifyKeyIncrease = 0;
        long long sumModifyKeyDecrease = 0;
        long long sumSize = 0;

        // Każdy pomiar startuje od świeżo zbudowanej struktury o tym samym rozmiarze N,
        // dzięki czemu kolejne operacje nie wpływają na siebie wzajemnie.
        const auto measureOperation = [&](const std::vector<int>& values, auto&& operation) {
            Queue queue;
            fillQueueForBenchmark(queue, values, size);
            const auto start = Clock::now();
            operation(queue);
            const auto end = Clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        };

        for (int attempt = 0; attempt < kBenchmarkAttempts; ++attempt) {
            // Dla każdej próby przygotowujemy zestaw danych zależny od rozmiaru i numeru próby.
            const auto values = generateBenchmarkData(size, size + 4, benchmarkSeed(size, attempt));
            const int targetValue = values[size / 2];
            const int increasedPriority = values[size * 2] + size * 5;
            const int decreasedPriority = values[size * 2 + 1] % (size + 10);
            const int insertValue = values[size * 2 + 2];
            const int insertPriority = values[size * 2 + 3];

            sumInsert += measureOperation(values, [&](Queue& q) {
                q.insert(insertValue, insertPriority);
            });
            sumExtractMax += measureOperation(values, [](Queue& q) {
                q.extractMax();
            });
            sumPeek += measureOperation(values, [](Queue& q) {
                q.peek();
            });
            sumModifyKeyIncrease += measureOperation(values, [&](Queue& q) {
                q.modifyKey(targetValue, increasedPriority);
            });
            sumModifyKeyDecrease += measureOperation(values, [&](Queue& q) {
                q.modifyKey(targetValue, decreasedPriority);
            });
            sumSize += measureOperation(values, [](Queue& q) {
                // `volatile` zapobiega usunięciu wywołania przez optymalizator.
                volatile const auto measuredSize = q.size();
                (void)measuredSize;
            });
        }

        const long long avgInsert = sumInsert / kBenchmarkAttempts;
        const long long avgExtractMax = sumExtractMax / kBenchmarkAttempts;
        const long long avgPeek = sumPeek / kBenchmarkAttempts;
        const long long avgModifyKeyIncrease = sumModifyKeyIncrease / kBenchmarkAttempts;
        const long long avgModifyKeyDecrease = sumModifyKeyDecrease / kBenchmarkAttempts;
        const long long avgSize = sumSize / kBenchmarkAttempts;

        file << "insert," << size << ',' << avgInsert << '\n';
        file << "extractMax," << size << ',' << avgExtractMax << '\n';
        file << "peek," << size << ',' << avgPeek << '\n';
        file << "increaseKey," << size << ',' << avgModifyKeyIncrease << '\n';
        file << "decreaseKey," << size << ',' << avgModifyKeyDecrease << '\n';
        file << "returnSize," << size << ',' << avgSize << '\n';

        appendMeasurement(txtFile, structureName, "insert", size, avgInsert);
        appendMeasurement(txtFile, structureName, "extractMax", size, avgExtractMax);
        appendMeasurement(txtFile, structureName, "peek", size, avgPeek);
        appendMeasurement(txtFile, structureName, "increaseKey", size, avgModifyKeyIncrease);
        appendMeasurement(txtFile, structureName, "decreaseKey", size, avgModifyKeyDecrease);
        appendMeasurement(txtFile, structureName, "returnSize", size, avgSize);
    }
}

template <typename Queue>
void menuPriorityQueue(const std::string& title,
                       const std::string& autosaveFile,
                       const std::string& manualSaveFile) {
    // Menu operuje na jednej instancji kolejki, dopóki użytkownik nie wróci do menu głównego.
    Queue queue;

    while (true) {
        std::cout << "\n================= " << title << " =================\n";
        std::cout << "1. insert(e, p)\n";
        std::cout << "2. extract-max()\n";
        std::cout << "3. find-max() / peek()\n";
        std::cout << "4. modify-key(e, p)\n";
        std::cout << "5. return-size\n";
        std::cout << "6. Zapisz aktualny stan do CSV\n";
        std::cout << "7. Wczytaj dane z pliku CSV\n";
        std::cout << "8. Wygeneruj losowe dane\n";
        std::cout << "9. Wyczysc strukture\n";
        std::cout << "10. Wstecz do menu glownego\n";
        std::cout << "=====================================================\n";

        const int choice = readInt("Twoj wybor: ");
        clearConsole();

        switch (choice) {
        case 1: {
            const int value = readInt("Podaj element: ");
            const int priority = readInt("Podaj priorytet: ");
            queue.insert(value, priority);
            // Po każdej zmianie zapisujemy stan pomocniczy, żeby łatwo było podejrzeć dane.
            saveQueueState(queue, autosaveFile);
            std::cout << "Element zostal dodany.\n";
            break;
        }
        case 2: {
            const auto removed = queue.extractMax();
            if (removed.has_value()) {
                saveQueueState(queue, autosaveFile);
                std::cout << "Usunieto element: " << removed->value
                          << ", priorytet: " << removed->priority << '\n';
            } else {
                std::cout << "Struktura jest pusta.\n";
            }
            break;
        }
        case 3:
            printEntry<Queue>(queue.peek());
            break;
        case 4: {
            const int value = readInt("Podaj wartosc elementu do zmiany: ");
            const int priority = readInt("Podaj nowy priorytet: ");
            if (queue.modifyKey(value, priority)) {
                saveQueueState(queue, autosaveFile);
                std::cout << "Priorytet zostal zmieniony.\n";
            } else {
                std::cout << "Nie znaleziono podanego elementu.\n";
            }
            break;
        }
        case 5:
            std::cout << "Rozmiar kolejki: " << queue.size() << '\n';
            break;
        case 6:
            // Osobny zapis pozwala ręcznie wyeksportować aktualny stan pod wybraną nazwą pliku.
            if (queue.saveToCSV(manualSaveFile)) {
                std::cout << "Zapisano stan do pliku: " << manualSaveFile << '\n';
            } else {
                std::cout << "Blad zapisu pliku.\n";
            }
            break;
        case 7: {
            std::string filename;
            std::cout << "Podaj nazwe pliku CSV: ";
            std::cin >> filename;
            if (queue.loadFromCSV(filename)) {
                saveQueueState(queue, autosaveFile);
                std::cout << "Dane wczytane poprawnie.\n";
            } else {
                std::cout << "Nie udalo sie wczytac pliku.\n";
            }
            break;
        }
        case 8: {
            const int size = readInt("Podaj liczbe elementow: ");
            queue.generateRandom(static_cast<std::size_t>(size), 1, size * 5 + 10);
            saveQueueState(queue, autosaveFile);
            std::cout << "Wygenerowano losowe dane.\n";
            break;
        }
        case 9:
            queue.clear();
            saveQueueState(queue, autosaveFile);
            std::cout << "Struktura zostala wyczyszczona.\n";
            break;
        case 10:
            return;
        default:
            std::cout << "Nieprawidlowy wybor.\n";
            break;
        }
    }
}
}  // namespace menu_utils

void menuBinaryHeap() {
    menu_utils::menuPriorityQueue<HeapQueue>("KOPIEC BINARNY MAX", "kopiec_autosave.csv", "kopiec.csv");
}

void menuLinkedListPriorityQueue() {
    menu_utils::menuPriorityQueue<ListQueue>("KOLEJKA PRIORYTETOWA NA LISCIE", "lista_autosave.csv", "lista.csv");
}

void menuGlowne() {
    while (true) {
        std::cout << "\n==================== MENU GLOWNE ====================\n";
        std::cout << "1. Kopiec binarny MAX\n";
        std::cout << "2. Druga implementacja kolejki priorytetowej\n";
        std::cout << "3. Badania wydajnosciowe i zapis CSV\n";
        std::cout << "4. Wyjscie\n";
        std::cout << "=====================================================\n";

        const int choice = menu_utils::readInt("Twoj wybor: ");
        menu_utils::clearConsole();

        switch (choice) {
        case 1:
            menuBinaryHeap();
            break;
        case 2:
            menuLinkedListPriorityQueue();
            break;
        case 3:
            runBenchmarks();
            break;
        case 4:
            std::cout << "Zakonczenie programu.\n";
            return;
        default:
            std::cout << "Nieprawidlowy wybor.\n";
            break;
        }
    }
}

void runBenchmarks() {
    std::cout << "\n================= BADANIA WYDAJNOSCIOWE =================\n";
    std::cout << "Uruchamianie testow dla dwoch implementacji kolejki priorytetowej MAX.\n";
    std::cout << "Wyniki zostana zapisane do plikow CSV oraz do pliku pomiary.txt.\n";
    std::cout << "Mierzone operacje: insert, extract-max, peek, increase-key, decrease-key, return-size.\n";
    std::cout << "=========================================================\n";

    std::ofstream txtFile("pomiary.txt", std::ios::trunc);
    if (!txtFile.is_open()) {
        std::cerr << "Blad otwarcia pliku pomiary.txt\n";
        return;
    }
    txtFile << "Struktura;Operacja;Rozmiar;SredniCzas_ns\n";
    txtFile.close();

    // Najpierw zapisujemy informacje pomocnicze, a potem uruchamiamy pomiary dla obu struktur.
    menu_utils::writeSeedsForSize("seedy_100000.txt", 100000);

    menu_utils::benchmarkQueue<HeapQueue>("KopiecBinarny", "benchmark_kopiec.csv");
    menu_utils::benchmarkQueue<ListQueue>("ListaJednokierunkowaPQ", "benchmark_lista.csv");

    std::cout << "\nBadania zakonczone. Utworzone pliki:\n";
    std::cout << "- pomiary.txt\n";
    std::cout << "- seedy_100000.txt\n";
    std::cout << "- benchmark_kopiec.csv\n";
    std::cout << "- benchmark_lista.csv\n";
}
