#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

// We store `kSize` numbers using `kBits`, excluding ones that are divisible by
// several given smallest primes.
constexpr struct Indexer {
  static constexpr ptrdiff_t kSize = 2 * 3 * 5 * 7 * 11 * 13;
  static constexpr ptrdiff_t kBits = 1 * 2 * 4 * 6 * 10 * 12;  // phi(kSize)

  constexpr Indexer() : indexOf(), atIndex() {
    ptrdiff_t i = 0;
    for (int n = 0; n < kSize; n++) {
      if ((n % 2 == 0) || (n % 3 == 0) || (n % 5 == 0) || (n % 7 == 0) ||
          (n % 11 == 0) || (n % 13 == 0)) {
        indexOf[n] = -1;
      } else {
        atIndex[i] = n;
        indexOf[n] = i++;
      }
    }
  }

  ptrdiff_t indexOf[kSize];
  int64_t atIndex[kBits];
} kIndexer;

// Computes `-x mod p`.
constexpr int64_t MinusMod(int64_t x, int64_t p) {
  return p - 1 - (x + p - 1) % p;
}

// Represents a sieved range of `offset * Indexer::kSize` numbers starting at
// `offset`.
class Range {
 public:
  Range(int64_t offset, int64_t size)
      : offset_(offset),
        size_(size),
        max_(size * Indexer::kSize),
        bitsets_(std::make_unique<std::bitset<Indexer::kBits>[]>(size)) {
    if (offset == 0) {  // We don't consider 1 to be a prime.
      bitsets_[0].set(0);
    }
  }

  void Sieve(const int64_t p) { Sieve(p, MinusMod(offset_, p)); }
  void Sieve(const int64_t p, int64_t offset) {
    for (; offset < max_; offset += p) {
      const ptrdiff_t index = kIndexer.indexOf[offset % Indexer::kSize];
      if (index >= 0) {
        bitsets_[offset / Indexer::kSize].set(index);
      }
    }
  }

  // Runs a given function for all numbers in the range that are marked as
  // primes.
  template <typename F>
  void ForPrimes(F&& f) const {
    for (ptrdiff_t j = 0; j < size_; j++) {
      auto& bitset = bitsets_[j];
      const int64_t offset = j * Indexer::kSize;
      for (ptrdiff_t index = 0; index < Indexer::kBits; index++) {
        if (!bitset[index]) {
          std::forward<F>(f)(offset + kIndexer.atIndex[index]);
        }
      }
    }
  }

 private:
  const int64_t offset_;
  // The number of numbers represented by this range.
  const int64_t max_;
  // The number of elements in `bitsets_`.
  const int64_t size_;
  std::unique_ptr<std::bitset<Indexer::kBits>[]> bitsets_;
};

// Outputs `p` to stdout as a 64-bit little-endian number.
void PrintLittleEndian(int64_t p) {
  char out[8];
  for (size_t i = 0; i < 8; i++) {
    out[i] = p & 0xff;
    p >>= 8;
  }
  fwrite(&out, sizeof(out), 1, stdout);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr
        << "Emits primes as 64-bit little-endian binary numbers to stdout."
        << std::endl
        << std::endl;
    std::cerr << "Please specify an upper bound as the only parameter."
              << std::endl;
    return 1;
  }
  const int64_t maximum = std::stoll(argv[1]);
  // The number of `Indexer::kSize` blocks we need to represent all primes
  // <= sqrt(maximum).
  const size_t kSieveSize =
      static_cast<size_t>(std::ceil(std::sqrt(maximum) / Indexer::kSize));
  for (int i : {2, 3, 5, 7, 11, 13}) {
    if (i > maximum) {
      exit(0);
    }
    PrintLittleEndian(i);
  }
  Range primes(0, kSieveSize);
  // Seed the initial range of primes. It is OK to run the `ForPrimes` loop and
  // rune `primes.Sieve` inside it - primes are processed while they're
  // generated.
  primes.ForPrimes([maximum, &primes](const int64_t p) {
    if (p > maximum) {
      exit(0);
    }
    PrintLittleEndian(p);
    primes.Sieve(p, 17 * p);
  });
  // This loop can be easily parallelized to utilize all cores, if desired.
  const size_t block_count = (maximum + kSieveSize - 1) / kSieveSize;
  for (int64_t block = 1; block < block_count; block++) {
    const int64_t offset = block * kSieveSize * Indexer::kSize;
    Range range(offset, kSieveSize);
    primes.ForPrimes([&range](const int64_t p) { range.Sieve(p); });
    range.ForPrimes([offset, maximum](const int64_t p) {
      const int64_t n = p + offset;
      if (n > maximum) {
        exit(0);
      }
      PrintLittleEndian(n);
    });
  }
  return 0;
}
