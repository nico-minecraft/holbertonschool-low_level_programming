#include "main.h"

/**
 * is_prime_helper - checks if a number is prime, using recursion, by
 * testing candidate divisors
 * @n: the number to be tested
 * @divisor: the candidate divisor being tested
 *
 * Return: 1 if n is prime, otherwise 0
 */
int is_prime_helper(int n, int divisor)
{
	if (divisor * divisor > n)
		return (1);

	if (n % divisor == 0)
		return (0);

	return (is_prime_helper(n, divisor + 1));
}

/**
 * is_prime_number - checks if an integer is a prime number, using
 * recursion
 * @n: the integer to be tested
 *
 * Return: 1 if n is a prime number, otherwise 0
 */
int is_prime_number(int n)
{
	if (n < 2)
		return (0);

	return (is_prime_helper(n, 2));
}
