/**
 * _atoi - converts a string to an integer
 * @s: the string to convert
 *
 * Return: the converted integer, or 0 if no digits are found
 */
int _atoi(char *s)
{
	int i;
	int sign;
	int started;
	unsigned int num;

	i = 0;
	sign = 1;
	started = 0;
	num = 0;

	while (s[i] != '\0')
	{
	if (started)
	{
		if (s[i] >= '0' && s[i] <= '9')
			num = (num * 10) + (unsigned int)(s[i] - '0');
		else
			break;
	}
	else
	{
		if (s[i] == '-')
			sign *= -1;
		else if (s[i] >= '0' && s[i] <= '9')
		{
			started = 1;
			num = (unsigned int)(s[i] - '0');
		}
		/* '+' and any other character before the number: ignored */
	}
	i++;
	}

	if (sign == -1)
		num = -num;

	return ((int)num);
}
