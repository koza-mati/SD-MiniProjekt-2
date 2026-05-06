# SD-MiniProjekt-2

Projekt z kursu Struktury Danych: kolejka priorytetowa typu MAX w C++. Program udostępnia dwie implementacje tej samej struktury (kopiec binarny oraz listę jednokierunkową), menu konsolowe oraz moduł pomiarów wydajności zapisywanych do plików CSV i TXT.

## Cel projektu

Celem projektu jest:

- implementacja kolejki priorytetowej typu MAX,
- porównanie dwóch reprezentacji tej samej struktury danych,
- obsługa podstawowych operacji kolejki priorytetowej przez wspólny interfejs,
- wykonanie pomiarów czasu działania operacji dla różnych rozmiarów danych,
- zapis i odczyt stanu kolejki z plików CSV.

Zaimplementowane operacje:

- `insert(e, p)` - dodaje element `e` o priorytecie `p`,
- `extractMax()` / `extract-max()` - usuwa i zwraca element o najwyższym priorytecie,
- `peek()` / `find-max()` - zwraca element o najwyższym priorytecie bez usuwania,
- `modifyKey(e, p)` / `modify-key(e, p)` - zmienia priorytet pierwszego znalezionego elementu o wartości `e`,
- `size()` / `return-size` - zwraca liczbę elementów,
- `empty()` - sprawdza, czy kolejka jest pusta,
- `clear()` - usuwa wszystkie elementy,
- `saveToCSV()` i `loadFromCSV()` - zapis i odczyt danych,
- `generateRandom()` - wypełnienie struktury losowymi danymi.

## Technologie i ograniczenia

Projekt jest napisany w C++20. Wykorzystuje standardową bibliotekę C++ m.in. do strumieni plikowych, pomiaru czasu, generowania liczb losowych, `std::optional`, `std::string` i `std::vector` w benchmarkach.

Kod korzysta również z `windows.h` oraz `system("cls")`, dlatego menu jest przygotowane z myślą o uruchamianiu w konsoli Windows.

Implementacje kolejek są klasami szablonowymi rozdzielonymi na pliki `.hpp` i `.tpp`.

## Struktura projektu

Katalog `src/` zawiera:

- `main.cpp` - punkt startowy programu,
- `menu.hpp` - deklaracje funkcji menu i benchmarków,
- `menu.cpp` - menu konsolowe, zapis/odczyt CSV z poziomu programu oraz benchmarki,
- `IPriorityQueue.hpp` - wspólny interfejs kolejki priorytetowej,
- `PriorityQueueCommon.hpp` - wspólna struktura wpisu i porównywanie priorytetów,
- `DynamicArray.hpp` - własna tablica dynamiczna używana przez kopiec,
- `BinaryHeapPriorityQueue.hpp` - deklaracja kolejki opartej na kopcu binarnym,
- `BinaryHeapPriorityQueue.tpp` - implementacja kolejki na kopcu binarnym,
- `LinkedListPriorityQueue.hpp` - deklaracja kolejki opartej na liście jednokierunkowej,
- `LinkedListPriorityQueue.tpp` - implementacja kolejki na liście jednokierunkowej.

## Wspólny model danych

Element kolejki jest reprezentowany przez `PriorityQueueEntry<T, PriorityType>`:

- `value` - przechowywana wartość,
- `priority` - priorytet; im większa wartość, tym wyższy priorytet,
- `order` - numer kolejności wstawienia.

Funkcja `hasHigherPriority()` najpierw porównuje `priority`, a przy remisie wybiera wpis z mniejszym `order`. Dzięki temu elementy o takim samym priorytecie są obsługiwane zgodnie z FIFO.

## DynamicArray i pojemność bufora

Kopiec binarny nie korzysta z `std::vector`, tylko z własnej klasy `DynamicArray`. Klasa przechowuje trzy pola:

- `data_` - wskaźnik na zaalokowany bufor,
- `size_` - liczba faktycznie zapisanych elementów,
- `capacity_` - pojemność bufora, czyli liczba elementów, które mogą zmieścić się w aktualnie zaalokowanej tablicy bez ponownej alokacji pamięci.

`capacity_` nie oznacza liczby elementów w kolejce. Liczbę elementów zwraca `size()`, które korzysta z `size_`. Pojemność jest zapasem pamięci potrzebnym po to, aby nie tworzyć nowej tablicy przy każdym pojedynczym `insert`.

Gdy `pushBack()` chce dodać element, a `size_ >= capacity_`, wywoływana jest metoda `grow()`. Tworzy ona nowy bufor o dwukrotnie większej pojemności, kopiuje do niego dotychczasowe elementy, zwalnia stary bufor i aktualizuje `capacity_`. Dzięki temu pojedyncze rozszerzenie jest kosztowne, ale zdarza się rzadko, a dodawanie na koniec tablicy ma złożoność amortyzowaną `O(1)`.

W naszym kodzie `capacity_` ma więc zadanie organizacyjne i wydajnościowe: kontroluje, kiedy trzeba powiększyć bufor kopca, oraz ogranicza liczbę realokacji pamięci podczas wielu operacji `insert`.

## Implementacja: kopiec binarny

`BinaryHeapPriorityQueue` implementuje kolejkę jako kopiec binarny typu MAX. W przeciwieństwie do typowej implementacji ze `std::vector`, kopiec w tym projekcie korzysta z własnej klasy `DynamicArray`.

Dla indeksu `i`:

- rodzic znajduje się pod indeksem `(i - 1) / 2`,
- lewe dziecko pod `2 * i + 1`,
- prawe dziecko pod `2 * i + 2`.

Działanie operacji:

- `insert` dodaje element na koniec tablicy i wykonuje `heapifyUp`,
- `extractMax` usuwa korzeń, przenosi ostatni element na początek i wykonuje `heapifyDown`,
- `peek` zwraca korzeń kopca,
- `modifyKey` liniowo szuka pierwszego elementu o podanej wartości, zmienia priorytet i naprawia kopiec w górę albo w dół,
- `clear` zeruje rozmiar tablicy i licznik `nextOrder_`.

Złożoność:

- `insert` - `O(log n)`,
- `extractMax` - `O(log n)`,
- `peek` - `O(1)`,
- `modifyKey` - `O(n)` na wyszukanie + `O(log n)` na naprawę kopca,
- `size` / `empty` - `O(1)`,
- `saveToCSV` - `O(n)`,
- `loadFromCSV` - `O(n log n)`, bo elementy są wczytywane przez `insert`.

## Implementacja: lista jednokierunkowa

`LinkedListPriorityQueue` implementuje kolejkę jako nieuporządkowaną listę jednokierunkową. Nowy element jest dodawany na początek listy, a maksimum jest wyszukiwane dopiero przy `peek` albo `extractMax`.

Każdy węzeł przechowuje:

- jeden `PriorityQueueEntry`,
- wskaźnik na następny węzeł.

Działanie operacji:

- `insert` tworzy nowy węzeł na początku listy,
- `extractMax` liniowo wyszukuje najlepszy węzeł, odpina go i zwraca jego dane,
- `peek` liniowo wyszukuje najlepszy węzeł bez usuwania,
- `modifyKey` liniowo szuka pierwszego elementu o podanej wartości i zmienia jego priorytet,
- `clear` iteracyjnie zwalnia wszystkie węzły.

Złożoność:

- `insert` - `O(1)`,
- `extractMax` - `O(n)`,
- `peek` - `O(n)`,
- `modifyKey` - `O(n)`,
- `size` / `empty` - `O(1)`,
- `saveToCSV` - `O(n)`,
- `loadFromCSV` - `O(n)`.

## Menu programu

Po uruchomieniu programu dostępne jest menu główne:

1. kopiec binarny MAX,
2. kolejka priorytetowa na liście jednokierunkowej,
3. badania wydajnościowe i zapis CSV,
4. wyjście.

W menu konkretnej struktury można:

- dodać element z priorytetem,
- usunąć maksimum,
- podejrzeć maksimum,
- zmienić priorytet elementu,
- sprawdzić rozmiar,
- zapisać aktualny stan do CSV,
- wczytać dane z pliku CSV,
- wygenerować losowe dane,
- wyczyścić strukturę.

Po operacjach modyfikujących program automatycznie zapisuje stan pomocniczy:

- `kopiec_autosave.csv` dla kopca,
- `lista_autosave.csv` dla listy.

Ręczny zapis z menu tworzy:

- `kopiec.csv` dla kopca,
- `lista.csv` dla listy.

## Generowanie danych

Metoda `generateRandom(count, minPriority, maxPriority)` czyści aktualną strukturę i tworzy `count` nowych elementów.

W menu priorytety są losowane z zakresu:

```text
1 .. size * 5 + 10
```

Wartości elementów są losowane z zakresu:

```text
1 .. count * 10 + 100
```

Do generowania używane są `std::random_device`, `std::mt19937` i `std::uniform_int_distribution`.

## Zapis i odczyt CSV

Obie implementacje zapisują pliki w formacie:

```csv
value,priority,order
10,45,0
7,12,1
15,45,2
```

Ważna uwaga: obecna implementacja `loadFromCSV()` odczytuje z pliku tylko `value` i `priority`, a następnie odbudowuje strukturę przez `insert`. Kolumna `order` jest zapisywana, ale nie jest odtwarzana przy wczytywaniu. Po wczytaniu elementy dostają nowe numery `order` zgodnie z kolejnością rekordów w pliku.

## Badania wydajnościowe

Benchmarki są uruchamiane z menu głównego przez opcję `Badania wydajnościowe i zapis CSV`.

Mierzone operacje:

- `insert`,
- `extractMax`,
- `peek`,
- `increaseKey`,
- `decreaseKey`,
- `returnSize`.

Rozmiary struktur używane w benchmarkach:

```text
10000, 20000, 40000, 80000, 100000, 160000, 320000, 640000
```

Dla każdego rozmiaru każda operacja jest mierzona 100 razy. Przed pojedynczym pomiarem tworzona jest nowa struktura wypełniona tym samym zestawem danych dla danej próby. Dane testowe są generowane deterministycznie na podstawie funkcji `benchmarkSeed(size, attempt)`.

Pliki wynikowe:

- `pomiary.txt` - zbiorcze zestawienie wyników,
- `benchmark_kopiec.csv` - wyniki dla kopca binarnego,
- `benchmark_lista.csv` - wyniki dla listy jednokierunkowej,
- `seedy_100000.txt` - lista seedów dla rozmiaru 100000.

Format plików CSV z benchmarkami:

```csv
Operation,Size,AverageTime_ns
insert,10000,123
extractMax,10000,456
```

## Kompilacja

Przykładowa komenda kompilacji:

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -I src src/main.cpp src/menu.cpp -o app.exe
```

Na Windows można też nazwać plik wynikowy bez rozszerzenia, ale obecny katalog projektu zawiera artefakt `app.exe`.

## Uruchomienie

Windows:

```bash
app.exe
```

Git Bash / MSYS / podobne środowisko:

```bash
./app.exe
```

## Najważniejsze decyzje projektowe

- obie implementacje realizują wspólny interfejs `IPriorityQueue`,
- kopiec używa własnej tablicy dynamicznej `DynamicArray`,
- lista jest nieuporządkowana, co daje szybkie `insert`, ale wolniejsze `peek` i `extractMax`,
- przy równych priorytetach obie struktury stosują FIFO przez pole `order`,
- `modifyKey` zmienia pierwszy znaleziony element o wskazanej wartości,
- menu i benchmarki pracują na tych samych publicznych operacjach interfejsu.

## Podsumowanie

Projekt realizuje kolejkę priorytetową typu MAX w dwóch wariantach: przez kopiec binarny oraz przez nieuporządkowaną listę jednokierunkową. Zawiera menu konsolowe, losowe generowanie danych, zapis/odczyt CSV oraz benchmarki porównujące koszty podstawowych operacji.
