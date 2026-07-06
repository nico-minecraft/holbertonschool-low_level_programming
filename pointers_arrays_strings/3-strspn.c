/**
 * _strspn - gets the length of a prefix substring
 * @s: the string to check
 * @accept: the set of bytes to match against
 *
 * Return: the number of bytes in the initial segment of s which
 * consist only of bytes from accept
 */
unsigned int _strspn(char *s, char *accept)
{
	unsigned int count;
	int i, j;
	int found;

	count = 0;
	i = 0;

	while (s[i] != '\0')
	{
		found = 0;
		j = 0;
		while (accept[j] != '\0')
		{
			if (s[i] == accept[j])
			{
				found = 1;
				break;
			}
			j++;
		}

		if (!found)
			break;

		count++;
		i++;
	}

	return (count);
}
