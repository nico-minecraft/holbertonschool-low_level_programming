#include <stdio.h>
/**
 * main - Prints the whole alphabet in lowercase and the uppercase
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
