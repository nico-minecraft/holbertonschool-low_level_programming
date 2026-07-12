#include <stdlib.h>
/**
* *malloc_checked - Allocated memory and checks it
* Return: Returns 98 if succesful
* @b: Memory to be allocated
*/
void *malloc_checked(unsigned int b)
{
	void *ptr;

	ptr = malloc(b);
	if (ptr == NULL)
		exit(98);
	return (ptr);
}
