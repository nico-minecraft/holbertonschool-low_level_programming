#include <stdio.h>
/**
 * main - Prints all base 16 numbers
 * Return: should always be 0
 */
int main(void)
{
	char n = '0';
	char nhex = 'a';

	while (n <= '9')
	{
		putchar(n);
		n++;
	}
		while (nhex <= 'f')
	{
		putchar(nhex);
		nhex++;
	}
	putchar('\n');
	return (0);
}
