#include "main.h"
/**
 * print_most_numbers - Prints all single digit numbers except for 2 and 4
 * Return: should always be 0
 */
void print_most_numbers(void)
{
	char n = '0';

	while (n <= '9')
	{
		if (n != '2' && n != '4')
		{
			_putchar(n);
		}
		n++;
	}
	_putchar('\n');
}
