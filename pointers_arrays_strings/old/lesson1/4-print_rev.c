#include <unistd.h>

/**
 * print_rev - Prints a string in reverse
 * @str: The string to print
 */
void print_rev(char *str)
{
	int len;

	len = 0;
	while (str[len] != '\0')
		len++;

	len--;
	while (len >= 0)
	{
		write(1, &str[len], 1);
			len--;
	}
	write(1, "\n", 1);
}
