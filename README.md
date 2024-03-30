# Zillion primes

*Disclaimer:* This is not an officially supported Google product.

This program uses the [Sieve of
Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) to generate
primes up to a given bound.

On a standard low-end Intel Core i5 it produces ~7.9M primes per second.

It uses approximately _0.024√n_ bytes of RAM to compute primes up to _n._

## Output

The program emits primes to _stdout_ encoded as 64-bit binary [little-endian]
numbers.

This format is easily readable by programs, and can be even [`mmap`]-ed as an
`int64_t[]` array. Also allows to quickly read an N-th prime.

[`mmap`]: https://en.wikipedia.org/wiki/Mmap
[little-endian]: https://en.wikipedia.org/w/index.php?title=Endianness&oldid=1212636685#Numbers

## Compilation

```shell
$ clang++ -O3 sieve.cc -o sieve
```

`g++` works just as well, it just produces slightly slower (~15%) binary.

## Testing

The first 50.000.000 primes should satisfy the following hash:

```sh
$ ./sieve 982451653 | sha256sum
17d28fa909939b450dbd6b8923a1001c61bdc8cedb5e44a07f9f90b4d36ab279  -
```
