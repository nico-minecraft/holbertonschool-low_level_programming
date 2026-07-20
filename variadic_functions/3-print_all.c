#include <stdio.h>
#include <stdarg.h>
/**
 * print_all - Prints numbers
 * Return: Returns 0
 * @format: number of integers passed to the function
 */
void print_all(const char * const format, ...)
{
va_list args;
unsigned int i;
char *sep;
char *str;

va_start(args, format);
sep = "";
i = 0;

while (format != NULL && format[i])
{
switch (format[i])
{
case 'c':
printf("%s%c", sep, va_arg(args, int));
sep = ", ";
break;
case 'i':
printf("%s%d", sep, va_arg(args, int));
sep = ", ";
break;
case 'f':
printf("%s%f", sep, va_arg(args, double));
sep = ", ";
break;
case 's':
str = va_arg(args, char *);
if (str == NULL)
str = "(nil)";
printf("%s%s", sep, str);
sep = ", ";
break;
default:
break;
}
i++;
}

va_end(args);
printf("\n");
}
