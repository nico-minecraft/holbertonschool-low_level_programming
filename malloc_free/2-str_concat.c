#include <stdlib.h>

/**
* *str_concat - Concatenates two separate strings
* Return: Returns a pointer to the concatenated string
* @s1: String to be concatenated
* @s2: Concatenate to be strung
*/

char *str_concat(char *s1, char *s2)
{
char *result;
int len1;
int len2;
int i;
int j;

len1 = 0;
while (s1 != NULL && s1[len1] != '\0')
	len1++;

len2 = 0;
while (s2 != NULL && s2[len2] != '\0')
	len2++;

result = malloc(sizeof(char) * (len1 + len2 + 1));
if (result == NULL)
	return (NULL);

i = 0;
while (i < len1)
{
	result[i] = s1[i];
	i++;
}

j = 0;
while (j < len2)
{
	result[i] = s2[j];
	i++;
	j++;
	}

result[i] = '\0';

return (result);
}
