#include "main.h"

/**
 * puts_half - prints the second half of a string, followed by a newline
 * @str: the string to print
 */
void puts_half(char *str)
{
	int len;
	int start;
	int i;

	len = 0;
	while (str[len] != '\0')
		len++;

	if (len % 2 == 0)
		start = len / 2;
	else
		start = (len - ((len + 1) / 2));

	i = start;
	while (str[i] != '\0')
	{
		_putchar(str[i]);
		i++;
	}
	_putchar('\n');
}