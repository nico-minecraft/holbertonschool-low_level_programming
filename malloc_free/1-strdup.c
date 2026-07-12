#include <stdlib.h>

/**
* _strdup - Returns a pointer to a duplicate of the string
* Return: Returns an initialized array
* @str: String to be duplicated
* @*dup: Pointer to the duplicated string
*/

char *_strdup(char *str)
{
	char *dup;
	int len;
	int i;

	if (str == NULL)
		return (NULL);

	len = 0;
	while (str[len] != '\0')
		len++;

	dup = malloc(sizeof(char) * (len + 1));
	if (dup == NULL)
		return (NULL);

	for (i = 0; i < len; i++)
		dup[i] = str[i];
	dup[len] = '\0';

	return (dup);
}
