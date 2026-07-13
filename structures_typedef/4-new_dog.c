#include "dog.h"
#include <stdlib.h>

/**
 * dup_str - Duplicates a string into newly allocated memory
 * @str: the string to duplicate
 *
 * Return: pointer to the newly allocated copy, or NULL on failure
 */
char *dup_str(char *str)
{
	char *copy;
	int len;
	int i;

	len = 0;
	while (str[len] != '\0')
		len++;

	copy = malloc(sizeof(char) * (len + 1));
	if (copy == NULL)
		return (NULL);

	for (i = 0; i < len; i++)
		copy[i] = str[i];
	copy[len] = '\0';

	return (copy);
}

/**
 * new_dog - Creates a new dog
 * @name: name of the dog
 * @age: age of the dog
 * @owner: owner of the dog
 *
 * Return: pointer to the new dog, or NULL on failure
 */
dog_t *new_dog(char *name, float age, char *owner)
{
	dog_t *d;

	d = malloc(sizeof(dog_t));
	if (d == NULL)
		return (NULL);

	d->name = dup_str(name);
	if (d->name == NULL)
	{
		free(d);
		return (NULL);
	}

	d->owner = dup_str(owner);
	if (d->owner == NULL)
	{
		free(d->name);
		free(d);
		return (NULL);
	}

	d->age = age;

	return (d);
}
