# SD-MiniProjekt-2

Projekt z kursu Struktury Danych: kolejka priorytetowa typu MAX w C++. Program udostepnia dwie implementacje tej samej struktury, menu konsolowe oraz modul pomiarow wydajnosci zapisywanych do plikow CSV i TXT.

## Cel projektu

Celem projektu jest:

- implementacja kolejki priorytetowej typu MAX,
- porownanie dwoch reprezentacji tej samej struktury danych,
- obsluga podstawowych operacji kolejki priorytetowej przez wspolny interfejs,
- wykonanie pomiarow czasu dzialania operacji dla roznych rozmiarow danych,
- zapis i odczyt stanu kolejki z plikow CSV.

Zaimplementowane operacje:

- `insert(e, p)` - dodaje element `e` o priorytecie `p`,
- `extractMax()` / `extract-max()` - usuwa i zwraca element o najwyzszym priorytecie,
- `peek()` / `find-max()` - zwraca element o najwyzszym priorytecie bez usuwania,
- `modifyKey(e, p)` / `modify-key(e, p)` - zmienia priorytet pierwszego znalezionego elementu o wartosci `e`,
- `size()` / `return-size` - zwraca liczbe elementow,
- `empty()` - sprawdza, czy kolejka jest pusta,
- `clear()` - usuwa wszystkie elementy,
- `saveToCSV()` i `loadFromCSV()` - zapis i odczyt danych,
- `generateRandom()` - wypelnienie struktury losowymi danymi.

## Technologie i ograniczenia

Projekt jest napisany w C++20. Wykorzystuje standardowa biblioteke C++ m.in. do strumieni plikowych, pomiaru czasu, generowania liczb losowych, `std::optional`, `std::string` i `std::vector` w benchmarkach.

Kod korzysta rowniez z `windows.h` oraz `system("cls")`, dlatego menu jest przygotowane z mysla o uruchamianiu w konsoli Windows.

Implementacje kolejek sa klasami szablonowymi rozdzielonymi na pliki `.hpp` i `.tpp`.

## Struktura projektu

Katalog `src/` zawiera:

- `main.cpp` - punkt startowy programu,
- `menu.hpp` - deklaracje funkcji menu i benchmarkow,
- `menu.cpp` - menu konsolowe, zapis/odczyt CSV z poziomu programu oraz benchmarki,
- `IPriorityQueue.hpp` - wspolny interfejs kolejki priorytetowej,
- `PriorityQueueCommon.hpp` - wspolna struktura wpisu i porownywanie priorytetow,
- `DynamicArray.hpp` - wlasna tablica dynamiczna uzywana przez kopiec,
- `BinaryHeapPriorityQueue.hpp` - deklaracja kolejki opartej na kopcu binarnym,
- `BinaryHeapPriorityQueue.tpp` - implementacja kolejki na kopcu binarnym,
- `LinkedListPriorityQueue.hpp` - deklaracja kolejki opartej na liscie jednokierunkowej,
- `LinkedListPriorityQueue.tpp` - implementacja kolejki na liscie jednokierunkowej.

## Wspolny model danych

Element kolejki jest reprezentowany przez `PriorityQueueEntry<T, PriorityType>`:

- `value` - przechowywana wartosc,
- `priority` - priorytet; im wieksza wartosc, tym wyzszy priorytet,
- `order` - numer kolejnosci wstawienia.

Funkcja `hasHigherPriority()` najpierw porownuje `priority`, a przy remisie wybiera wpis z mniejszym `order`. Dzieki temu elementy o takim samym priorytecie sa obslugiwane zgodnie z FIFO.

## DynamicArray i pojemnosc bufora

Kopiec binarny nie korzysta z `std::vector`, tylko z wlasnej klasy `DynamicArray`. Klasa przechowuje trzy pola:

- `data_` - wskaznik na zaalokowany bufor,
- `size_` - liczba faktycznie zapisanych elementow,
- `capacity_` - pojemnosc bufora, czyli liczba elementow, ktore moga zmiescic sie w aktualnie zaalokowanej tablicy bez ponownej alokacji pamieci.

`capacity_` nie oznacza liczby elementow w kolejce. Liczbe elementow zwraca `size()`, ktore korzysta z `size_`. Pojemnosc jest zapasem pamieci potrzebnym po to, aby nie tworzyc nowej tablicy przy kazdym pojedynczym `insert`.

Gdy `pushBack()` chce dodac element, a `size_ >= capacity_`, wywolywana jest metoda `grow()`. Tworzy ona nowy bufor o dwukrotnie wiekszej pojemnosci, kopiuje do niego dotychczasowe elementy, zwalnia stary bufor i aktualizuje `capacity_`. Dzieki temu pojedyncze rozszerzenie jest kosztowne, ale zdarza sie rzadko, a dodawanie na koniec tablicy ma zlozonosc amortyzowana `O(1)`.

W naszym kodzie `capacity_` ma wiec zadanie organizacyjne i wydajnosciowe: kontroluje, kiedy trzeba powiekszyc bufor kopca, oraz ogranicza liczbe realokacji pamieci podczas wielu operacji `insert`.

## Implementacja: kopiec binarny

`BinaryHeapPriorityQueue` implementuje kolejke jako kopiec binarny typu MAX. W przeciwienstwie do typowej implementacji ze `std::vector`, kopiec w tym projekcie korzysta z wlasnej klasy `DynamicArray`.

Dla indeksu `i`:

- rodzic znajduje sie pod indeksem `(i - 1) / 2`,
- lewe dziecko pod `2 * i + 1`,
- prawe dziecko pod `2 * i + 2`.

Dzialanie operacji:

- `insert` dodaje element na koniec tablicy i wykonuje `heapifyUp`,
- `extractMax` usuwa korzen, przenosi ostatni element na poczatek i wykonuje `heapifyDown`,
- `peek` zwraca korzen kopca,
- `modifyKey` liniowo szuka pierwszego elementu o podanej wartosci, zmienia priorytet i naprawia kopiec w gore albo w dol,
- `clear` zeruje rozmiar tablicy i licznik `nextOrder_`.

Zlozonosc:

- `insert` - `O(log n)`,
- `extractMax` - `O(log n)`,
- `peek` - `O(1)`,
- `modifyKey` - `O(n)` na wyszukanie + `O(log n)` na naprawe kopca,
- `size` / `empty` - `O(1)`,
- `saveToCSV` - `O(n)`,
- `loadFromCSV` - `O(n log n)`, bo elementy sa wczytywane przez `insert`.

## Implementacja: lista jednokierunkowa

`LinkedListPriorityQueue` implementuje kolejke jako nieuporzadkowana liste jednokierunkowa. Nowy element jest dodawany na poczatek listy, a maksimum jest wyszukiwane dopiero przy `peek` albo `extractMax`.

Kazdy wezel przechowuje:

- jeden `PriorityQueueEntry`,
- wskaznik na nastepny wezel.

Dzialanie operacji:

- `insert` tworzy nowy wezel na poczatku listy,
- `extractMax` liniowo wyszukuje najlepszy wezel, odpina go i zwraca jego dane,
- `peek` liniowo wyszukuje najlepszy wezel bez usuwania,
- `modifyKey` liniowo szuka pierwszego elementu o podanej wartosci i zmienia jego priorytet,
- `clear` iteracyjnie zwalnia wszystkie wezly.

Zlozonosc:

- `insert` - `O(1)`,
- `extractMax` - `O(n)`,
- `peek` - `O(n)`,
- `modifyKey` - `O(n)`,
- `size` / `empty` - `O(1)`,
- `saveToCSV` - `O(n)`,
- `loadFromCSV` - `O(n)`.

## Menu programu

Po uruchomieniu programu dostepne jest menu glowne:

1. kopiec binarny MAX,
2. kolejka priorytetowa na liscie jednokierunkowej,
3. badania wydajnosciowe i zapis CSV,
4. wyjscie.

W menu konkretnej struktury mozna:

- dodac element z priorytetem,
- usunac maksimum,
- podejrzec maksimum,
- zmienic priorytet elementu,
- sprawdzic rozmiar,
- zapisac aktualny stan do CSV,
- wczytac dane z pliku CSV,
- wygenerowac losowe dane,
- wyczyscic strukture.

Po operacjach modyfikujacych program automatycznie zapisuje stan pomocniczy:

- `kopiec_autosave.csv` dla kopca,
- `lista_autosave.csv` dla listy.

Reczny zapis z menu tworzy:

- `kopiec.csv` dla kopca,
- `lista.csv` dla listy.

## Generowanie danych

Metoda `generateRandom(count, minPriority, maxPriority)` czysci aktualna strukture i tworzy `count` nowych elementow.

W menu priorytety sa losowane z zakresu:

```text
1 .. size * 5 + 10
```

Wartosci elementow sa losowane z zakresu:

```text
1 .. count * 10 + 100
```

Do generowania uzywane sa `std::random_device`, `std::mt19937` i `std::uniform_int_distribution`.

## Zapis i odczyt CSV

Obie implementacje zapisuja pliki w formacie:

```csv
value,priority,order
10,45,0
7,12,1
15,45,2
```

Wazna uwaga: obecna implementacja `loadFromCSV()` odczytuje z pliku tylko `value` i `priority`, a nastepnie odbudowuje strukture przez `insert`. Kolumna `order` jest zapisywana, ale nie jest odtwarzana przy wczytywaniu. Po wczytaniu elementy dostaja nowe numery `order` zgodnie z kolejnoscia rekordow w pliku.

## Badania wydajnosciowe

Benchmarki sa uruchamiane z menu glownego przez opcje `Badania wydajnosciowe i zapis CSV`.

Mierzone operacje:

- `insert`,
- `extractMax`,
- `peek`,
- `increaseKey`,
- `decreaseKey`,
- `returnSize`.

Rozmiary struktur uzywane w benchmarkach:

```text
10000, 20000, 40000, 80000, 100000, 160000, 320000, 640000
```

Dla kazdego rozmiaru kazda operacja jest mierzona 100 razy. Przed pojedynczym pomiarem tworzona jest nowa struktura wypelniona tym samym zestawem danych dla danej proby. Dane testowe sa generowane deterministycznie na podstawie funkcji `benchmarkSeed(size, attempt)`.

Pliki wynikowe:

- `pomiary.txt` - zbiorcze zestawienie wynikow,
- `benchmark_kopiec.csv` - wyniki dla kopca binarnego,
- `benchmark_lista.csv` - wyniki dla listy jednokierunkowej,
- `seedy_100000.txt` - lista seedow dla rozmiaru 100000.

Format plikow CSV z benchmarkami:

```csv
Operation,Size,AverageTime_ns
insert,10000,123
extractMax,10000,456
```

## Kompilacja

Przykladowa komenda kompilacji:

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -I src src/main.cpp src/menu.cpp -o app.exe
```

Na Windows mozna tez nazwac plik wynikowy bez rozszerzenia, ale obecny katalog projektu zawiera artefakt `app.exe`.

## Uruchomienie

Windows:

```bash
app.exe
```

Git Bash / MSYS / podobne srodowisko:

```bash
./app.exe
```

## Najwazniejsze decyzje projektowe

- obie implementacje realizuja wspolny interfejs `IPriorityQueue`,
- kopiec uzywa wlasnej tablicy dynamicznej `DynamicArray`,
- lista jest nieuporzadkowana, co daje szybkie `insert`, ale wolniejsze `peek` i `extractMax`,
- przy rownych priorytetach obie struktury stosuja FIFO przez pole `order`,
- `modifyKey` zmienia pierwszy znaleziony element o wskazanej wartosci,
- menu i benchmarki pracuja na tych samych publicznych operacjach interfejsu.

## Podsumowanie

Projekt realizuje kolejke priorytetowa typu MAX w dwoch wariantach: przez kopiec binarny oraz przez nieuporzadkowana liste jednokierunkowa. Zawiera menu konsolowe, losowe generowanie danych, zapis/odczyt CSV oraz benchmarki porownujace koszty podstawowych operacji.
