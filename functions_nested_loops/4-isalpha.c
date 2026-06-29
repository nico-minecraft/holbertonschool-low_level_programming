#include "main.h"
/**
 * _isalpha - Determines if the character is alphanumeric
 * Return: 1 if it is lowercase, 0 if it is not
 * @c: is an int variable for the letter to be checked
 */
int _isalpha(int c)
{
	if (c >= 'A' && c <= 'z')
	{
		return (1);
	}

	return (0);
}
