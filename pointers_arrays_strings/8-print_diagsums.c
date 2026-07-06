#include "main.h"
#include <stdio.h>

/**
 * print_diagsums - prints the sum of the two diagonals of a square
 * matrix of integers
 * @a: pointer to the first element of the matrix (flattened)
 * @size: the size of the matrix (size x size)
 *
 * Return: nothing
 */
void print_diagsums(int *a, int size)
{
int i;
int sum1, sum2;

sum1 = 0;
sum2 = 0;

for (i = 0; i < size; i++)
{
sum1 += a[i * size + i];
sum2 += a[i * size + (size - 1 - i)];
}

printf("First diagonal: %d, Second diagonal: %d\n", sum1, sum2);
}
