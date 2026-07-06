/**
 * leet - encodes a string into 1337
 * @s: the string to encode
 *
 * Return: a pointer to the modified string
 */
char *leet(char *s)
{
	char *from = "aAeEoOtTlL";
	char *to = "4433007711";
	int i, j;

	i = 0;
	while (s[i] != '\0')
	{
		j = 0;
		while (from[j] != '\0')
		{
			if (s[i] == from[j])
			{
				s[i] = to[j];
				break;
			}
			j++;
		}
		i++;
	}

	return (s);
}
