/**
 * rev_string - reverses a string in place
 * @s: the string to reverse
 *
 * Return: nothing
 */
void rev_string(char *s)
{
	int start, end;
	char temp;

	start = 0;
	end = 0;

	while (s[end] != '\0')
		end++;
	end--;

	while (start < end)
	{
		temp = s[start];
		s[start] = s[end];
		s[end] = temp;
		start++;
		end--;
	}
}
