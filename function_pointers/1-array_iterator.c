#include <stdio.h>
/**
 * array_iterator - Iterates over an array
 * @array: Array
 * @size: Size of the array
 * @action: Action pointer
 */
void array_iterator(int *array, size_t size, void (*action)(int))
{
size_t i;

for (i = 0; i < size; i++)
action(array[i]);
}
