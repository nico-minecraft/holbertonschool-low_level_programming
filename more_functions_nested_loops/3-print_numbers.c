#include "main.h"
/**
 * print_numbers - Prints all single digit numbers as strings
 * Return: should always be 0
 */
void print_numbers(void)
{
	char n = '0';

	while (n <= '9')
	{
		_putchar(n);
		n++;
	}
	_putchar('\n');
}
