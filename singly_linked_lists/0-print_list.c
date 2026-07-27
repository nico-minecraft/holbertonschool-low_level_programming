#include "lists.h"

/**
 * print_list - prints all the elements of a list_t list
 * @h: pointer to the head of the list
 *
 * Return: the number of nodes in the list
 */
size_t print_list(const list_t *h)
{
	size_t count = 0;
	unsigned int len, div;
	int i;

	while (h != NULL)
	{
		_putchar('[');
		len = h->len;
		div = 1;
		while (len / div >= 10)
			div *= 10;
		while (div > 0)
		{
			_putchar('0' + (len / div) % 10);
			div /= 10;
		}
		_putchar(']');
		_putchar(' ');
		if (h->str == NULL)
		{
			_putchar('(');
			_putchar('n');
			_putchar('i');
			_putchar('l');
			_putchar(')');
		}
		else
		{
			for (i = 0; h->str[i]; i++)
				_putchar(h->str[i]);
		}
		_putchar('\n');
		h = h->next;
		count++;
	}

	return (count);
}