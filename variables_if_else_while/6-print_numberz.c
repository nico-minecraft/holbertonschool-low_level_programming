#include <stdio.h>
/**
 * main - Prints all single digit numbers as integers
 * Return: should always be 0
 */
int main(void)
{
	int n = 0;

	while (n <= 9)
	{
		putchar(n + '0');
		n++;
	}
	putchar('\n');
	return (0);
}
