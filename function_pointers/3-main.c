#include "3-calc.h"

/**
 * main - performs a simple operation on two integers
 * @argc: argument count
 * @argv: argument vector
 * Return: 0 on success
 */
int main(int argc, char *argv[])
{
int num1, num2, result;
int (*f)(int, int);

if (argc != 4)
{
printf("Error\n");
exit(98);
}

f = get_op_func(argv[2]);
if (f == NULL)
{
printf("Error\n");
exit(99);
}

num1 = atoi(argv[1]);
num2 = atoi(argv[3]);

if ((*argv[2] == '/' || *argv[2] == '%') && num2 == 0)
{
printf("Error\n");
exit(100);
}

result = f(num1, num2);
printf("%d\n", result);

return (0);
}
