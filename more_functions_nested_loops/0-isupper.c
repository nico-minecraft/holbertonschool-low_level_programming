#include "main.h"
/**
 * _islower - Determines if the character is lower case or not
 * Return: 1 if it is lowercase, 0 if it is not
 * @c: is an int variable for the letter to be checked
 */
int _isupper(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return (1);
	}

	return (0);
}
