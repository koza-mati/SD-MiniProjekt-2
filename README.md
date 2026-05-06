# SD-MiniProjekt-2

Projekt z kursu Struktury Danych przedstawiający implementację kolejki priorytetowej typu MAX w języku C++. W projekcie zaimplementowano dwie różne struktury realizujące ten sam abstrakcyjny typ danych, przygotowano menu tekstowe do obsługi operacji oraz moduł badań wydajnościowych zapisujący wyniki do plików CSV.

## Cel projektu

Celem projektu jest:

- implementacja kolejki priorytetowej typu MAX,
- porównanie dwóch różnych reprezentacji tej samej struktury danych,
- wykonanie pomiarów czasu działania podstawowych operacji,
- zapis wyników badań do plików tekstowych i CSV.

Zgodnie z treścią zadania zaimplementowano operacje:

- `insert(e, p)` - dodanie elementu `e` o priorytecie `p`,
- `extract-max()` - usunięcie i zwrócenie elementu o najwyższym priorytecie,
- `find-max()` / `peek()` - podejrzenie elementu o najwyższym priorytecie bez usuwania,
- `modify-key(e, p)` - zmiana priorytetu wybranego elementu,
- `return-size` - zwrócenie liczby elementów w strukturze.

## Użyte technologie

Projekt został napisany w:

- `C++20`
- biblioteka standardowa C++ (`vector`, `optional`, `fstream`, `chrono`, `random`, `string`, `limits`)
- `windows.h` wykorzystywane jedynie do wygodnej pracy z menu konsolowym na Windows

Kod jest podzielony na pliki nagłówkowe `hpp`, plik implementacyjny `cpp` dla warstwy aplikacyjnej oraz pliki `tpp` dla implementacji klas szablonowych.

## Zawartość projektu

Struktura katalogu `src/`:

- `main.cpp` - punkt startowy programu
- `menu.hpp` - deklaracje funkcji menu
- `menu.cpp` - obsługa menu, zapis CSV oraz benchmarki
- `IPriorityQueue.hpp` - wspólny interfejs kolejek priorytetowych
- `PriorityQueueCommon.hpp` - wspólne definicje wpisu kolejki i porównywania priorytetów
- `BinaryHeapPriorityQueue.hpp` - deklaracja kolejki priorytetowej opartej na kopcu
- `BinaryHeapPriorityQueue.tpp` - implementacja kolejki na kopcu binarnym
- `LinkedListPriorityQueue.hpp` - deklaracja kolejki priorytetowej opartej na liście jednokierunkowej
- `LinkedListPriorityQueue.tpp` - implementacja kolejki na liście jednokierunkowej

## Wspólny model danych

Każdy element kolejki jest reprezentowany przez strukturę:

- `value` - przechowywana wartość,
- `priority` - priorytet elementu,
- `order` - numer kolejności wstawienia.

Pole `order` zostało dodane po to, aby przy równych priorytetach zachować zasadę FIFO. Oznacza to, że jeśli dwa elementy mają ten sam priorytet, jako "lepszy" traktowany jest ten, który został dodany wcześniej.

## Implementacja 1 - kopiec binarny

Pierwsza implementacja kolejki priorytetowej korzysta z kopca binarnego typu MAX.

### Reprezentacja

Kopiec jest przechowywany w tablicy dynamicznej (`std::vector`). Dla indeksu `i`:

- rodzic znajduje się pod indeksem `(i - 1) / 2`,
- lewe dziecko pod `2 * i + 1`,
- prawe dziecko pod `2 * i + 2`.

### Działanie operacji

- `insert` - dodaje element na koniec tablicy i wykonuje `heapifyUp`
- `extractMax` - usuwa korzeń, przenosi ostatni element na początek i wykonuje `heapifyDown`
- `peek` - zwraca element z korzenia kopca
- `modifyKey` - zmienia priorytet wskazanego elementu i naprawia kopiec w górę lub w dół
- `size` - zwraca liczbę elementów w wektorze

### Złożoność

- `insert` - `O(log n)`
- `extractMax` - `O(log n)`
- `peek` - `O(1)`
- `modifyKey` - `O(n)` na znalezienie elementu + `O(log n)` na naprawę kopca
- `size` - `O(1)`

## Implementacja 2 - lista jednokierunkowa

Druga implementacja kolejki priorytetowej wykorzystuje nieuporządkowaną listę jednokierunkową.

### Reprezentacja

Każdy węzeł przechowuje:

- jeden wpis kolejki,
- wskaźnik na następny element.

Nowe elementy dodawane są zawsze na początek listy.

### Działanie operacji

- `insert` - wstawia nowy element na początek listy
- `extractMax` - przeszukuje listę liniowo, znajduje maksimum i usuwa odpowiedni węzeł
- `peek` - przeszukuje listę liniowo i zwraca element o najwyższym priorytecie
- `modifyKey` - wyszukuje pierwszy element o podanej wartości i zmienia jego priorytet
- `size` - zwraca aktualny licznik elementów

### Złożoność

- `insert` - `O(1)`
- `extractMax` - `O(n)`
- `peek` - `O(n)`
- `modifyKey` - `O(n)`
- `size` - `O(1)`

## Interfejs programu

Program uruchamia menu tekstowe w konsoli. Użytkownik może:

- wybrać implementację opartą na kopcu,
- wybrać implementację opartą na liście,
- wykonać badania wydajnościowe,
- zakończyć program.

Dla każdej implementacji dostępne są operacje:

- dodanie elementu z priorytetem,
- usunięcie maksimum,
- podejrzenie maksimum,
- zmiana priorytetu,
- odczyt rozmiaru,
- zapis aktualnego stanu do CSV,
- wczytanie danych z CSV,
- losowe wygenerowanie danych,
- wyczyszczenie struktury.

## Generowanie danych

Losowe dane generowane są osobno dla obu implementacji.

Przyjęto, że:

- wartości elementów są losowane z dodatniego zakresu liczb całkowitych,
- priorytety są losowane z zakresu większego niż rozmiar struktury,
- w menu generowanie odbywa się z zakresu `1` do `size * 5 + 10`.

To spełnia założenie z treści projektu, że zakres priorytetów powinien być kilkukrotnie większy od liczby elementów.

## FIFO przy równych priorytetach

W projekcie przyjęto zasadę FIFO dla elementów o tym samym priorytecie.

Realizacja:

- każdy element otrzymuje numer `order`,
- podczas porównania najpierw sprawdzany jest `priority`,
- jeśli priorytety są równe, wyżej oceniany jest element z mniejszym `order`.

Takie rozwiązanie działa tak samo dla kopca i dla implementacji listowej.

## Badania wydajnościowe

Program zawiera moduł benchmarków porównujący obie implementacje.

### Mierzone operacje

- `insert`
- `extractMax`
- `peek`
- `increaseKey`
- `decreaseKey`
- `returnSize`

### Sposób pomiaru

- pomiary wykonywane są dla kilku rozmiarów struktur,
- dla każdego rozmiaru każda operacja jest wykonywana 100 razy,
- każdy pomiar startuje od nowo zbudowanej struktury o tym samym rozmiarze,
- dane testowe są generowane deterministycznie na podstawie ziarna (`seed`),
- na koniec zapisywana jest średnia wartość czasu w nanosekundach.

### Pliki wynikowe

Po uruchomieniu benchmarków program tworzy:

- `pomiary.txt` - zbiorcze zestawienie wyników
- `benchmark_kopiec.csv` - wyniki dla kopca binarnego
- `benchmark_lista.csv` - wyniki dla implementacji listowej
- `seedy_100000.txt` - zapis przykładowych ziaren losowania

## Zapis i odczyt CSV

Obie implementacje potrafią:

- zapisać aktualny stan kolejki do pliku CSV,
- odczytać dane z pliku CSV i odbudować strukturę.

Format zapisu:

```csv
value,priority,order
10,45,0
7,12,1
15,45,2
```

## Kompilacja

Przykładowa komenda kompilacji:

```bash
g++ -std=c++20 -Wall -Wextra -pedantic -I src src/main.cpp src/menu.cpp -o app.exe
```

## Uruchomienie

Po kompilacji program można uruchomić poleceniem:

```bash
./app.exe
```

lub w systemie Windows:

```bash
app.exe
```

## Najważniejsze decyzje projektowe

- zastosowano dwie różne reprezentacje kolejki priorytetowej, aby można było porównać ich zachowanie
- jedna z implementacji to kopiec binarny, zgodnie z wymaganiami projektu
- dla równych priorytetów przyjęto FIFO
- interfejs kolejek został ujednolicony przez `IPriorityQueue`
- menu i benchmarki korzystają z tych samych operacji dla obu struktur

## Podsumowanie

Projekt realizuje wymagania miniprojektu dotyczące kolejki priorytetowej typu MAX. Zawiera:

- dwie implementacje kolejki priorytetowej,
- obsługę wszystkich wymaganych operacji,
- generowanie danych losowych,
- pomiary wydajności,
- zapis wyników do plików CSV i TXT,
- czytelny podział na pliki odpowiedzialne za konkretne elementy systemu.

Projekt może służyć zarówno jako rozwiązanie zadania laboratoryjnego, jak i jako materiał do porównania kosztów operacji w różnych implementacjach kolejki priorytetowej.
