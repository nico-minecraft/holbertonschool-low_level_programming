#include <stdio.h>
/**
 * main - Prints all base 16 numbers
 * Return: should always be 0
 */
int main(void)
{
	int n = 48;

	while (n <= 59)
	{
		putchar(n);
		n++;
	}
	putchar('\n');
	return (0);
}
