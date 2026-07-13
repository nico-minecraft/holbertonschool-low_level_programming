#include "dog.h"
#include <stdlib.h>
#include <string.h>

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

	d = malloc(sizeof(dog_t));
	if (d == NULL)
		return (NULL);

	name_copy = malloc(sizeof(char) * (strlen(name) + 1));
	if (name_copy == NULL)
	{
		free(d);
		return (NULL);
	}
	strcpy(name_copy, name);

	owner_copy = malloc(sizeof(char) * (strlen(owner) + 1));
	if (owner_copy == NULL)
	{
		free(name_copy);
		free(d);
		return (NULL);
	}
	strcpy(owner_copy, owner);

	d->name = name_copy;
	d->age = age;
	d->owner = owner_copy;

	return (d);
}
