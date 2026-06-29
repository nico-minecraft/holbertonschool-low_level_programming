#include "main.h"
/**
 * _isdigit - Determines if the character is upper case or not
 * Return: 1 if it is lowercase, 0 if it is not
 * @c: is an int variable for the letter to be checked
 */
int _isdigit(int c)
{
	if (c >= '0' && c <= '9')
	{
		return (1);
	}

	return (0);
}
