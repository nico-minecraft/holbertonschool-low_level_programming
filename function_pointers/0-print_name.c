#include <stdio.h>
/**
 * print_name - Prints the name provided
 * @name: Name to print
 * @f: Pointer function
 */
void print_name(char *name, void (*f)(char *))
{
f(name);
}
