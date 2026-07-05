#include <unistd.h>

/**
 * _puts - prints a string to stdout, followed by a newline
 * @str: the string to print
 */
void _puts(char *str)
{
    int i;

    i = 0;
    while (str[i] != '\0')
    {
        write(1, &str[i], 1);
        i++;
    }
    write(1, "\n", 1);
}