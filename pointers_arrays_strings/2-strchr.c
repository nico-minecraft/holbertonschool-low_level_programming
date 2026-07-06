#include <stddef.h>

/**
 * _strchr - locates the first occurrence of a character in a string
 * @s: the string to search
 * @c: the character to locate
 *
 * Return: a pointer to the first occurrence of c in s, or NULL if
 * the character is not found
 */
char *_strchr(char *s, char c)
{
	int i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			return (&s[i]);
		i++;
	}

	if (c == '\0')
		return (&s[i]);

	return (NULL);
}
