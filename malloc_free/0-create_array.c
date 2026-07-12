#include <stdlib.h>

/**
 * *create_array - Initializes an empty array
 * Return: Returns an initialized array
 * @size: size of the array
 * @c: Character to fill the array with
 */

char *create_array(unsigned int size, char c)
{
	char *array;
	unsigned int i;

	if (size == 0)
		return (NULL);

	array = malloc(sizeof(char) * size);
	if (array == NULL)
		return (NULL);

	for (i = 0; i < size; i++)
		array[i] = c;

	return (array);
}
