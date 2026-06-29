#include "main.h"
/**
 * print_sign - Determines if the int is positive, negative, or 0
 * Return: -1 for negative, 0 for 0, +1 for positive
 * @n: is an int variable for the number to be checked
 */
int print_sign(int n)
{
	if (n >= 1)
	{
		_putchar('+');
		return (1);
	}

	if (n == 0)
	{
		_putchar('0');
		return (0);
	}

	if (n <= -1)
	{
		_putchar('-');
		return (-1);
	}

	return (0);
}
