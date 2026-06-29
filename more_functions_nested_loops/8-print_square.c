#include "main.h"
/**
 * print_square - prints a square using #
 * @size: size of square
 * Return: should always be 0
 */
void print_square(int size)
{
	int row, col;

	for (row = 0; row < size; row++)
	{
		for (col = 0; col < size; col++)
			_putchar('#');
		_putchar('\n');
	}
	if (size <= 0)
		_putchar('\n');
}
