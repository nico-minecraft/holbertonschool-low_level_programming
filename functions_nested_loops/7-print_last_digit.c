#include "main.h"
/**
 * print_last_digit - Prints the last digit of int n
 * @n: number to evaluate
 * Return: should always be 0
 */
int print_last_digit(int n)
{
	int last;

	last = n % 10;
	if (last < 0)
		last = -last;
	_putchar(last + '0');
	return (last);
}
