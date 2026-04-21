# SD-MiniProjekt-2

Projekt z kursu Struktury Danych przedstawiajacy implementacje kolejki priorytetowej typu MAX w jezyku C++. W projekcie zaimplementowano dwie rozne struktury realizujace ten sam abstrakcyjny typ danych, przygotowano menu tekstowe do obslugi operacji oraz modul badan wydajnosciowych zapisujacy wyniki do plikow CSV.

## Cel projektu

Celem projektu jest:

- implementacja kolejki priorytetowej typu MAX,
- porownanie dwoch roznych reprezentacji tej samej struktury danych,
- wykonanie pomiarow czasu dzialania podstawowych operacji,
- zapis wynikow badan do plikow tekstowych i CSV.

Zgodnie z trescia zadania zaimplementowano operacje:

- `insert(e, p)` - dodanie elementu `e` o priorytecie `p`,
- `extract-max()` - usuniecie i zwrocenie elementu o najwyzszym priorytecie,
- `find-max()` / `peek()` - podejrzenie elementu o najwyzszym priorytecie bez usuwania,
- `modify-key(e, p)` - zmiana priorytetu wybranego elementu,
- `return-size` - zwrocenie liczby elementow w strukturze.

## Uzyte technologie

Projekt zostal napisany w:

- `C++20`
- biblioteka standardowa C++ (`vector`, `optional`, `fstream`, `chrono`, `random`, `string`, `limits`)
- `windows.h` wykorzystywane jedynie do wygodnej pracy z menu konsolowym na Windows

Kod jest podzielony na pliki naglowkowe `hpp`, plik implementacyjny `cpp` dla warstwy aplikacyjnej oraz pliki `tpp` dla implementacji klas szablonowych.

## Zawartosc projektu

Struktura katalogu `src/`:

- `main.cpp` - punkt startowy programu
- `menu.hpp` - deklaracje funkcji menu
- `menu.cpp` - obsluga menu, zapis CSV oraz benchmarki
- `IPriorityQueue.hpp` - wspolny interfejs kolejek priorytetowych
- `PriorityQueueCommon.hpp` - wspolne definicje wpisu kolejki i porownywania priorytetow
- `BinaryHeapPriorityQueue.hpp` - deklaracja kolejki priorytetowej opartej na kopcu
- `BinaryHeapPriorityQueue.tpp` - implementacja kolejki na kopcu binarnym
- `LinkedListPriorityQueue.hpp` - deklaracja kolejki priorytetowej opartej na liscie jednokierunkowej
- `LinkedListPriorityQueue.tpp` - implementacja kolejki na liscie jednokierunkowej

## Wspolny model danych

Kazdy element kolejki jest reprezentowany przez strukture:

- `value` - przechowywana wartosc,
- `priority` - priorytet elementu,
- `order` - numer kolejnosci wstawienia.

Pole `order` zostalo dodane po to, aby przy rownych priorytetach zachowac zasade FIFO. Oznacza to, ze jesli dwa elementy maja ten sam priorytet, jako "lepszy" traktowany jest ten, ktory zostal dodany wczesniej.

## Implementacja 1 - kopiec binarny

Pierwsza implementacja kolejki priorytetowej korzysta z kopca binarnego typu MAX.

### Reprezentacja

Kopiec jest przechowywany w tablicy dynamicznej (`std::vector`). Dla indeksu `i`:

- rodzic znajduje sie pod indeksem `(i - 1) / 2`,
- lewe dziecko pod `2 * i + 1`,
- prawe dziecko pod `2 * i + 2`.

### Dzialanie operacji

- `insert` - dodaje element na koniec tablicy i wykonuje `heapifyUp`
- `extractMax` - usuwa korzen, przenosi ostatni element na poczatek i wykonuje `heapifyDown`
- `peek` - zwraca element z korzenia kopca
- `modifyKey` - zmienia priorytet wskazanego elementu i naprawia kopiec w gore lub w dol
- `size` - zwraca liczbe elementow w wektorze

### Zlozonosc

- `insert` - `O(log n)`
- `extractMax` - `O(log n)`
- `peek` - `O(1)`
- `modifyKey` - `O(n)` na znalezienie elementu + `O(log n)` na naprawe kopca
- `size` - `O(1)`

## Implementacja 2 - lista jednokierunkowa

Druga implementacja kolejki priorytetowej wykorzystuje nieuporzadkowana liste jednokierunkowa.

### Reprezentacja

Kazdy wezel przechowuje:

- jeden wpis kolejki,
- wskaznik na nastepny element.

Nowe elementy dodawane sa zawsze na poczatek listy.

### Dzialanie operacji

- `insert` - wstawia nowy element na poczatek listy
- `extractMax` - przeszukuje liste liniowo, znajduje maksimum i usuwa odpowiedni wezel
- `peek` - przeszukuje liste liniowo i zwraca element o najwyzszym priorytecie
- `modifyKey` - wyszukuje pierwszy element o podanej wartosci i zmienia jego priorytet
- `size` - zwraca aktualny licznik elementow

### Zlozonosc

- `insert` - `O(1)`
- `extractMax` - `O(n)`
- `peek` - `O(n)`
- `modifyKey` - `O(n)`
- `size` - `O(1)`

## Interfejs programu

Program uruchamia menu tekstowe w konsoli. Uzytkownik moze:

- wybrac implementacje oparta na kopcu,
- wybrac implementacje oparta na liscie,
- wykonac badania wydajnosciowe,
- zakonczyc program.

Dla kazdej implementacji dostepne sa operacje:

- dodanie elementu z priorytetem,
- usuniecie maksimum,
- podejrzenie maksimum,
- zmiana priorytetu,
- odczyt rozmiaru,
- zapis aktualnego stanu do CSV,
- wczytanie danych z CSV,
- losowe wygenerowanie danych,
- wyczyszczenie struktury.

## Generowanie danych

Losowe dane generowane sa osobno dla obu implementacji.

Przyjeto, ze:

- wartosci elementow sa losowane z dodatniego zakresu liczb calkowitych,
- priorytety sa losowane z zakresu wiekszego niz rozmiar struktury,
- w menu generowanie odbywa sie z zakresu `1` do `size * 5 + 10`.

To spelnia zalozenie z tresci projektu, ze zakres priorytetow powinien byc kilkukrotnie wiekszy od liczby elementow.

## FIFO przy rownych priorytetach

W projekcie przyjeto zasade FIFO dla elementow o tym samym priorytecie.

Realizacja:

- kazdy element otrzymuje numer `order`,
- podczas porownania najpierw sprawdzany jest `priority`,
- jesli priorytety sa rowne, wyzej oceniany jest element z mniejszym `order`.

Takie rozwiazanie dziala tak samo dla kopca i dla implementacji listowej.

## Badania wydajnosciowe

Program zawiera modul benchmarkow porownujacy obie implementacje.

### Mierzone operacje

- `insert`
- `extractMax`
- `peek`
- `increaseKey`
- `decreaseKey`
- `returnSize`

### Sposob pomiaru

- pomiary wykonywane sa dla kilku rozmiarow struktur,
- dla kazdego rozmiaru kazda operacja jest wykonywana 100 razy,
- kazdy pomiar startuje od nowo zbudowanej struktury o tym samym rozmiarze,
- dane testowe sa generowane deterministycznie na podstawie ziarna (`seed`),
- na koniec zapisywana jest srednia wartosc czasu w nanosekundach.

### Pliki wynikowe

Po uruchomieniu benchmarkow program tworzy:

- `pomiary.txt` - zbiorcze zestawienie wynikow
- `benchmark_kopiec.csv` - wyniki dla kopca binarnego
- `benchmark_lista.csv` - wyniki dla implementacji listowej
- `seedy_100000.txt` - zapis przykladowych ziaren losowania

## Zapis i odczyt CSV

Obie implementacje potrafia:

- zapisac aktualny stan kolejki do pliku CSV,
- odczytac dane z pliku CSV i odbudowac strukture.

Format zapisu:

```csv
value,priority,order
10,45,0
7,12,1
15,45,2
```

## Kompilacja

Przykladowa komenda kompilacji:

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -I src src/main.cpp src/menu.cpp -o app.exe
```

## Uruchomienie

Po kompilacji program mozna uruchomic poleceniem:

```bash
./app.exe
```

lub w systemie Windows:

```bash
app.exe
```

## Najwazniejsze decyzje projektowe

- zastosowano dwie rozne reprezentacje kolejki priorytetowej, aby mozna bylo porownac ich zachowanie
- jedna z implementacji to kopiec binarny, zgodnie z wymaganiami projektu
- dla rownych priorytetow przyjeto FIFO
- interfejs kolejek zostal ujednolicony przez `IPriorityQueue`
- menu i benchmarki korzystaja z tych samych operacji dla obu struktur

## Podsumowanie

Projekt realizuje wymagania miniprojektu dotyczace kolejki priorytetowej typu MAX. Zawiera:

- dwie implementacje kolejki priorytetowej,
- obsluge wszystkich wymaganych operacji,
- generowanie danych losowych,
- pomiary wydajnosci,
- zapis wynikow do plikow CSV i TXT,
- czytelny podzial na pliki odpowiedzialne za konkretne elementy systemu.

Projekt moze sluzyc zarowno jako rozwiazanie zadania laboratoryjnego, jak i jako material do porownania kosztow operacji w roznych implementacjach kolejki priorytetowej.
