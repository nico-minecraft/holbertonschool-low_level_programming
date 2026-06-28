#include <stdio.h>
/**
 * main - Prints the whole tebahpla
 * Return: should always be 0
 */
int main(void)
{
	char a = 'z';

	while (a >= 'a')
	{
		putchar(a);
		a--;
	}
	putchar('\n');
	return (0);
}
