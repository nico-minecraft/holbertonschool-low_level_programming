/**
 * _strcat - concatenates two strings
 * @dest: destination string
 * @src: source string to append
 * Return: a pointer to dest
 */
char *_strcat(char *dest, char *src)
{
	int dest_len;
	int i;

	dest_len = 0;
	while (dest[dest_len] != '\0')
		dest_len++;

	i = 0;
	while (src[i] != '\0')
	{
		dest[dest_len + i] = src[i];
		i++;
	}
	dest[dest_len + i] = '\0';

	return (dest);
}
