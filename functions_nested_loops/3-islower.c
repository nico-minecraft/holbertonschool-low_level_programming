#include "main.h"
/**
 * print_alphabet - Prints the whole alphabet
 * Return: should always be 0
 */
int _islower(char c)
{
	if (c >= 'a' && c <= 'z') {
		_putchar(1);
	}
	else {
		_putchar(0);
	}
/*	if ((c >= 'A') && (c <= 'Z')) {
		_putchar(1);
	} */

	return 0;
}
