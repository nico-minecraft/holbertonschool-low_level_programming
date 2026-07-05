#include <stdio.h>
#include "main.h"
/**
 * swap_int - Swaps the int variables
 * @a: Pointed variable to be swapped
 * @b: Pointed variable to be swapped
 */
void swap_int(int *a, int *b)
{
	int tempa, tempb;

	tempa = *a;
	tempb = *b;

	*a = tempb;
	*b = tempa;
}
