#include "main.h"
/**
 * print_triangle - Prints a triangle using #
 * @size: the size of the triangle
 * Return: should always be 0
 */
void print_triangle(int size)
{
	int row, col, space;

	for (row = 1; row <= size; row++)
	{
		for (space = 0; space < size - row; space++)
			_putchar(' ');
		for (col = 0; col < row; col++)
			_putchar('#');
		_putchar('\n');
	}
	if (size <= 0)
		_putchar('\n');
}
