#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/**
 * main - Prints the whole alphabet except for q and e
 * Return: should always be 0
 */
int main(void)
{
	char a = 'a';

	a--;
	while (a < 'z')
	{
		a++;
		if (a == 'e' || a == 'q')
			continue;
		putchar(a);
	}
	putchar('\n');
	return (0);
}
