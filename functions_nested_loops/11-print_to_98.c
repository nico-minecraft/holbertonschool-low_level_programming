#include "main.h"
#include <stdio.h>
/**
 * print_to_98 - Prints the number, approaching closer to 98,
 *               be it growing or shrinking
 * @n: number to reach 98
 */

void print_to_98(int n)
{

	do {
		printf("%d", n);
		if (n < 98)
		{
			n++;
			printf(", ");
		}
		else if (n > 98)
		{
			n--;
			printf(", ");
		}
	} while (n != 98);
}
