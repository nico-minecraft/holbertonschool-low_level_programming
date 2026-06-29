#include "main.h"
/**
 * print_diagonal - draws a diagonal line in the terminal
 * @n: ammount of / characters
 * Return: should always be 0
 */
void print_diagonal(int n)
{
	int row, space;

	for (row = 0; row < n; row++)
	{
		for (space = 0; space < row; space++)
			_putchar(' ');
		_putchar('\\');
		_putchar('\n');
	}
	if (n <= 0)
		_putchar('\n');
}
