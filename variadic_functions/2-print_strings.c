#include <stdio.h>
#include <stdarg.h>
/**
 * print_strings - Prints numbers
 * Return: Returns 0
 * @separator: string to be printed between numbers
 * @n: number of integers passed to the function
 */
void print_strings(const char *separator, const unsigned int n, ...)
{
va_list args;
unsigned int i;
char *str;

va_start(args, n);

for (i = 0; i < n; i++)
{
str = va_arg(args, char *);

if (str == NULL)
printf("(nil)");
else
printf("%s", str);

if (separator != NULL && i != n - 1)
printf("%s", separator);
}

va_end(args);

printf("\n");
}
