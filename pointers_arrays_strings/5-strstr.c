#include <stddef.h>

/**
 * _strstr - locates the first occurrence of a substring in a string
 * @haystack: the string to search
 * @needle: the substring to locate
 *
 * Return: a pointer to the beginning of the located substring, or
 * NULL if the substring is not found
 */
char *_strstr(char *haystack, char *needle)
{
	int i, j;

	i = 0;
	while (haystack[i] != '\0')
	{
		j = 0;
		while (needle[j] != '\0' && haystack[i + j] == needle[j])
			j++;

		if (needle[j] == '\0')
			return (&haystack[i]);

		i++;
	}

	if (needle[0] == '\0')
		return (&haystack[i]);

	return (NULL);
}
