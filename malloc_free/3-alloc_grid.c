#include <stdlib.h>
/**
* **alloc_grid - Allocates a grid of integers
* Return: Returns a pointer to a 2D array of integers
* @width: Width of the array
* @height: Height of the array
*/
#include <stdlib.h>

int **alloc_grid(int width, int height)
{
int **grid;
int i;
int j;

if (width <= 0 || height <= 0)
return (NULL);

grid = malloc(sizeof(int *) * height);
if (grid == NULL)
return (NULL);

for (i = 0; i < height; i++)
{
grid[i] = malloc(sizeof(int) * width);
if (grid[i] == NULL)
{
for (j = 0; j < i; j++)
free(grid[j]);
free(grid);
return (NULL);
}
}

for (i = 0; i < height; i++)
for (j = 0; j < width; j++)
grid[i][j] = 0;

return (grid);
}
