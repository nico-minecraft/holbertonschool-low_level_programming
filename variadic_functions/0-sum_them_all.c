#include <stdarg.h>
/**
 * sum_them_all - Prints the every hour and minute of the say
 * Return: Returns the sum
 * @n: number
 */
int sum_them_all(const unsigned int n, ...)
{
va_list args;
unsigned int i;
int sum;

if (n == 0)
return (0);

sum = 0;
va_start(args, n);

for (i = 0; i < n; i++)
sum += va_arg(args, int);

va_end(args);

return (sum);
}
