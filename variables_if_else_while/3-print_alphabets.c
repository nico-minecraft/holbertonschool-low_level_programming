#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/**
 * main - Prints the whole alphabet
 * Return: should always be 0
 */
int main(void)
{
	char a = 'a';
	char A = 'A';

	while (a <= 'z')
	{
		putchar(a);
		a++;
	}
	while (A <= 'Z')
	{
		putchar(A);
		A++;
	}

	putchar('\n');
	return (0);
}
