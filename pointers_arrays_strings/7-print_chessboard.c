#include "main.h"

/**
 * print_chessboard - prints a chessboard
 * @a: pointer to an array of 8 chars (i.e. an 8x8 2D array)
 *
 * Return: nothing
 */
void print_chessboard(char (*a)[8])
{
    int i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
            _putchar(a[i][j]);
        _putchar('\n');
    }
}