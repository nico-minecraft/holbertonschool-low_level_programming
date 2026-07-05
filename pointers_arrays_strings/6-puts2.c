#include "main.h"
/**
 * puts2 - prints every other character of a string, starting with the
 * first, followed by a newline
 * @str: the string to print
 */
void puts2(char *str)
{
    int i;

    i = 0;
    while (str[i] != '\0')
    {
        _putchar(str[i]);
        i += 2;
    }
    _putchar('\n');
}