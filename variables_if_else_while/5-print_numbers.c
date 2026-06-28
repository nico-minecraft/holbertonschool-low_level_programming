#include <stdio.h>
/**
 * main - Prints all single digit numbers
 * Return: should always be 0
 */
int main(void)
{
	char n = '0';

	while (n <= '9')
	{
		putchar(n);
		n++;
	}
	putchar('\n');
	return (0);
}
