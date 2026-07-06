#include <stdio.h>

/**
 * print_array - prints n elements of an array of integers, followed
 * by a newline
 * @a: pointer to the array
 * @n: number of elements to print
 */
void print_array(int *a, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (i > 0)
			printf(", ");
		printf("%d", a[i]);
	}
	printf("\n");
}
