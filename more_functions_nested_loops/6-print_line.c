#include "main.h"
/**
 * print_line - draws a straight line the size of n
 * @n: ammount of _ characters
 * Return: should always be 0
 */
void print_line(int n)
{
	int i;

	for (i = 0; i < n; i++)
		_putchar('_');
	_putchar('\n');
}
