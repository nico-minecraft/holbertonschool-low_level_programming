#include <stdio.h>
#include <stdlib.h>

/**
 * is_digit_str - checks that a string contains only digits
 * @s: the string to check
 * Return: 1 if all characters are digits, 0 otherwise
 */
int is_digit_str(char *s)
{
int i;

if (s[0] == '\0')
return (0);

for (i = 0; s[i] != '\0'; i++)
{
if (s[i] < '0' || s[i] > '9')
return (0);
}

return (1);
}

/**
 * main - adds positive numbers passed as arguments
 * @argc: argument count
 * @argv: argument vector
 * Return: 0 on success, 1 on error
 */
int main(int argc, char *argv[])
{
int i;
int sum;

sum = 0;

for (i = 1; i < argc; i++)
{
if (!is_digit_str(argv[i]))
{
printf("Error\n");
return (1);
}

sum += atoi(argv[i]);
}

printf("%d\n", sum);

return (0);
}
