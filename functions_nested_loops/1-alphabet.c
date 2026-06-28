#include "main.h"
/**
 * print_alphabet - Prints the whole alphabet
 * Return: should always be 0
 */
void print_alphabet(void)
{
	char a = 'a';

	while (a <= 'z')
	{
		_putchar(a);
		a++;
	}
	_putchar('\n');
}
