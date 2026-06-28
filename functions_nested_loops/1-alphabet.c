#include "main.h"
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
		_putchar(a);
	}
	_putchar('\n');
	return (0);
}
