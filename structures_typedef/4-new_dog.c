#include "dog.h"
#include <stdlib.h>

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
	char *name_copy;
	char *owner_copy;
	int len;
	int i;

	d = malloc(sizeof(dog_t));
	if (d == NULL)
		return (NULL);

	len = 0;
	while (name[len] != '\0')
		len++;
	name_copy = malloc(sizeof(char) * (len + 1));
	if (name_copy == NULL)
	{
		free(d);
		return (NULL);
	}
	for (i = 0; i <= len; i++)
		name_copy[i] = name[i];

	len = 0;
	while (owner[len] != '\0')
		len++;
	owner_copy = malloc(sizeof(char) * (len + 1));
	if (owner_copy == NULL)
	{
		free(name_copy);
		free(d);
		return (NULL);
	}
	for (i = 0; i <= len; i++)
		owner_copy[i] = owner[i];

	d->name = name_copy;
	d->age = age;
	d->owner = owner_copy;

	return (d);
}
