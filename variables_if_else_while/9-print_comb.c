#include <stdio.h>
#include <time.h>
/**
 * main - Prints all single digit numbers as strings, separated by commas
 * Return: should always be 0
 */

int main(void)
{
	char n = '0';
	char o = ',';
	char t = ' ';

	while (n <= '9')
	{
		putchar(n);
		if (n < '9')
		{
			putchar(o);
			putchar(t);
		}
		n++;
	}

	putchar('\n');
	return (0);
}
