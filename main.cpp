#include <iostream>
#include <sstream>
#include <chrono>
#include <random>
#include <string>

#include "src/ArraySequence.h"
#include "src/ListSequence.h"
#include "src/BitSequence.h"

using SeqInt = Sequence<int>;
using Clock  = std::chrono::high_resolution_clock;

// Статические функции (для совместимости с указателями на функции)
static int  fnDouble (int x)           { return x * 2; }
static bool fnIsEven (int x)           { return x % 2 == 0; }
static int  fnSum    (int acc, int x)  { return acc + x; }
static bool fnIsZero (int x)           { return x == 0; }

// Сортировка вставками для DynamicArray
static void insertionSort(DynamicArray<int>& arr) {
    int n = arr.GetSize();
    for (int i = 1; i < n; ++i) {
        int key = arr.Get(i);
        int j   = i - 1;
        while (j >= 0 && arr.Get(j) > key) {
            arr.Set(j + 1, arr.Get(j));
            --j;
        }
        arr.Set(j + 1, key);
    }
}

// Вспомогательные функции UI
static void printSeq(const SeqInt* s) {
    std::cout << "[";
    for (int i = 0; i < s->GetLength(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << s->Get(i);
    }
    std::cout << "]\n";
}

static void printTiming(std::chrono::nanoseconds ns) {
    std::cout << "  Time: " << ns.count() << " ns\n";
}

static SeqInt* makeSequence(int type) {
    switch (type) {
        case 0:  return new MutableArraySequence<int>();
        case 1:  return new ImmutableArraySequence<int>();
        case 2:  return new MutableListSequence<int>();
        case 3:  return new ImmutableListSequence<int>();
        default: return new MutableArraySequence<int>();
    }
}

// Медиана: один проход сбора + сортировка вставками
static double computeMedian(const SeqInt* s) {
    int n = s->GetLength();
    if (n == 0) throw std::runtime_error("empty sequence");
    DynamicArray<int> arr(n);
    for (int i = 0; i < n; ++i) arr.Set(i, s->Get(i));
    insertionSort(arr);
    return (n % 2 == 0)
        ? 0.5 * (arr.Get(n / 2 - 1) + arr.Get(n / 2))
        : static_cast<double>(arr.Get(n / 2));
}

// n! через однопроходный цикл
static long long computeFactorial(int n) {
    long long result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}

// Поэлементная сумма с отражением: sum[i] = s[i] + s[n-1-i]
static MutableArraySequence<int>* reflectionSum(const SeqInt* s) {
    int n = s->GetLength();
    auto* result = new MutableArraySequence<int>();
    for (int i = 0; i < n; ++i)
        result->Append(s->Get(i) + s->Get(n - 1 - i));
    return result;
}

// Справка
static void printHelp() {
    std::cout << R"(
=== Available Commands ===
  new              - create a new sequence
  show             - print current sequence
  get <i>          - get element at index i
  append <v>       - append value v
  prepend <v>      - prepend value v
  insert <v> <i>   - insert value v at index i
  concat           - concatenate with a second input sequence
  subseq <l> <r>   - get subsequence [l, r]
  map              - apply x*2 to each element
  where            - keep only even elements
  reduce           - compute sum via reduce
  zip              - zip with another sequence
  split            - split by zeros
  slice <i> <n>    - remove n elements starting at i (negative i = from end)
  median           - compute median (insertion sort)
  perms            - compute n! (number of permutations)
  reflect          - element-wise sum with reflection
  bit              - BitSequence operations demo
  help             - show this help
  quit             - exit
)";
}

int main() {
    std::cout << "=== Sequence Shell (C++20) ===\n";
    std::cout << "Type 'help' for a list of commands.\n\n";

    SeqInt* current = nullptr;
    int     seqType = 0;

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        try {
            if (cmd == "quit" || cmd == "exit") {
                break;

            } else if (cmd == "help") {
                printHelp();

            } else if (cmd == "new") {
                std::cout << "Select type:\n"
                          << "  0 = MutableArraySequence\n"
                          << "  1 = ImmutableArraySequence\n"
                          << "  2 = MutableListSequence\n"
                          << "  3 = ImmutableListSequence\n"
                          << "Type: ";
                std::cin >> seqType; std::cin.ignore();

                std::cout << "Input: 0=manual, 1=random\nChoice: ";
                int method; std::cin >> method; std::cin.ignore();

                // Сначала строим новый объект, удаляем старый только после успеха
                SeqInt* newSeq = makeSequence(seqType);

                if (method == 0) {
                    std::cout << "Enter integers (end with non-integer):\n";
                    int v;
                    while (std::cin >> v) {
                        SeqInt* next = newSeq->Append(v);
                        if (next != newSeq) { delete newSeq; newSeq = next; }
                    }
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                } else {
                    std::cout << "Size: "; int sz; std::cin >> sz;
                    std::cout << "Min: ";  int mn; std::cin >> mn;
                    std::cout << "Max: ";  int mx; std::cin >> mx;
                    std::cin.ignore();
                    std::mt19937 rng(std::random_device{}());
                    std::uniform_int_distribution<int> dist(mn, mx);
                    for (int i = 0; i < sz; ++i) {
                        SeqInt* next = newSeq->Append(dist(rng));
                        if (next != newSeq) { delete newSeq; newSeq = next; }
                    }
                }

                delete current;       // удаляем старую последовательность
                current = newSeq;
                std::cout << "Created: "; printSeq(current);

            } else if (cmd == "show") {
                if (!current) { std::cout << "No sequence. Use 'new' first.\n"; continue; }
                auto t0 = Clock::now();
                printSeq(current);
                printTiming(Clock::now() - t0);

            } else if (cmd == "get") {
                int idx; iss >> idx;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                std::cout << "  [" << idx << "] = " << current->Get(idx) << "\n";
                printTiming(Clock::now() - t0);

            } else if (cmd == "append") {
                int v; iss >> v;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                SeqInt* next = current->Append(v);
                if (next != current) { delete current; current = next; }
                printTiming(Clock::now() - t0);
                std::cout << "  Result: "; printSeq(current);

            } else if (cmd == "prepend") {
                int v; iss >> v;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                SeqInt* next = current->Prepend(v);
                if (next != current) { delete current; current = next; }
                printTiming(Clock::now() - t0);
                std::cout << "  Result: "; printSeq(current);

            } else if (cmd == "insert") {
                int v, idx; iss >> v >> idx;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                SeqInt* next = current->InsertAt(v, idx);
                if (next != current) { delete current; current = next; }
                printTiming(Clock::now() - t0);
                std::cout << "  Result: "; printSeq(current);

            } else if (cmd == "concat") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                std::cout << "Enter elements to concat (end with non-integer):\n";
                MutableArraySequence<int> other;
                int v;
                while (std::cin >> v) other.Append(v);
                std::cin.clear(); std::cin.ignore(1000, '\n');
                auto t0 = Clock::now();
                SeqInt* next = current->Concat(&other);
                if (next != current) { delete current; current = next; }
                printTiming(Clock::now() - t0);
                std::cout << "  Result: "; printSeq(current);

            } else if (cmd == "subseq") {
                int l, r; iss >> l >> r;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                auto* sub = current->GetSubsequence(l, r);
                printTiming(Clock::now() - t0);
                std::cout << "  Subsequence [" << l << "," << r << "]: ";
                printSeq(sub);
                delete sub;

            } else if (cmd == "map") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                auto* res = current->Map(fnDouble);
                printTiming(Clock::now() - t0);
                std::cout << "  map(x*2): "; printSeq(res);
                delete res;

            } else if (cmd == "where") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                auto* res = current->Where(fnIsEven);
                printTiming(Clock::now() - t0);
                std::cout << "  where(even): "; printSeq(res);
                delete res;

            } else if (cmd == "reduce") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                int s = current->Reduce(fnSum, 0);
                printTiming(Clock::now() - t0);
                std::cout << "  reduce(sum) = " << s << "\n";

            } else if (cmd == "zip") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                std::cout << "Enter second sequence (end with non-integer):\n";
                MutableArraySequence<int> other;
                int v;
                while (std::cin >> v) other.Append(v);
                std::cin.clear(); std::cin.ignore(1000, '\n');
                auto t0 = Clock::now();
                auto* zipped = Zip<int, int>(current, &other);
                printTiming(Clock::now() - t0);
                std::cout << "  Zipped pairs:\n";
                for (int i = 0; i < zipped->GetLength(); ++i)
                    std::cout << "    (" << zipped->Get(i).first
                              << ", "   << zipped->Get(i).second << ")\n";
                delete zipped;

            } else if (cmd == "split") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                auto* parts = current->Split(fnIsZero);
                printTiming(Clock::now() - t0);
                std::cout << "  Split by 0 (" << parts->GetLength() << " parts):\n";
                for (int i = 0; i < parts->GetLength(); ++i) {
                    std::cout << "    "; printSeq(parts->Get(i));
                    delete parts->Get(i);
                }
                delete parts;

            } else if (cmd == "slice") {
                int idx, n; iss >> idx >> n;
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                auto* res = current->Slice(idx, n);
                printTiming(Clock::now() - t0);
                std::cout << "  slice(" << idx << ", " << n << "): "; printSeq(res);
                delete res;

            } else if (cmd == "median") {
                if (!current || current->IsEmpty()) {
                    std::cout << "Empty sequence.\n"; continue;
                }
                auto t0 = Clock::now();
                double med = computeMedian(current);
                printTiming(Clock::now() - t0);
                std::cout << "  Median = " << med << "\n";

            } else if (cmd == "perms") {
                if (!current) { std::cout << "No sequence.\n"; continue; }
                auto t0 = Clock::now();
                long long f = computeFactorial(current->GetLength());
                printTiming(Clock::now() - t0);
                std::cout << "  Permutations(" << current->GetLength()
                          << ") = " << f << "\n";

            } else if (cmd == "reflect") {
                if (!current || current->IsEmpty()) {
                    std::cout << "Empty sequence.\n"; continue;
                }
                auto t0 = Clock::now();
                auto* res = reflectionSum(current);
                auto  elapsed = Clock::now() - t0;
                std::cout << "  Reflection sum: "; printSeq(res);
                delete res;
                printTiming(elapsed);

            } else if (cmd == "bit") {
                int ba[] = {1, 0, 1, 1, 0};
                int bb[] = {0, 1, 1, 0, 1};
                BitSequence bsa(ba, 5), bsb(bb, 5);
                std::cout << "  A   = " << bsa.ToString() << "\n";
                std::cout << "  B   = " << bsb.ToString() << "\n";
                auto* andR = bsa.AND(&bsb);
                auto* orR  = bsa.OR(&bsb);
                auto* xorR = bsa.XOR(&bsb);
                auto* notR = bsa.NOT();
                std::cout << "  A&B = " << andR->ToString() << "\n";
                std::cout << "  A|B = " << orR->ToString()  << "\n";
                std::cout << "  A^B = " << xorR->ToString() << "\n";
                std::cout << "  ~A  = " << notR->ToString() << "\n";
                delete andR; delete orR; delete xorR; delete notR;

            } else {
                std::cout << "Unknown command: " << cmd
                          << "  (type 'help' for list)\n";
            }

        } catch (const IndexOutOfRange& e) {
            std::cout << "Error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    // Освобождение памяти при любом выходе (quit, EOF, Ctrl+C)
    delete current;
    return 0;
}
