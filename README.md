# Zillion primes

This program uses the [Sieve of
Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) to generate
primes up to a given bound.

On a standard low-end Intel Core i5 it produces around 5M primes per second.

It uses approximately _0.047√n_ bytes of RAM to compute primes up to _n._

## Testing

The first 50.000.000 primes should satisfy the following hash:

```sh
$ ~/sieve 982451653 | sha256sum
17d28fa909939b450dbd6b8923a1001c61bdc8cedb5e44a07f9f90b4d36ab279  -
```
