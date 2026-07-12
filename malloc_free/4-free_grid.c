#include <stdlib.h>
/**
* free_grid - Frees the memory allocated by the grid
* Return: Returns 0
* @grid: Grid to be freed
* @height: Height of the array
*/
#include <stdlib.h>

void free_grid(int **grid, int height)
{
int i;

if (grid == NULL)
return;
for (i = 0; i < height; i++)
free(grid[i]);
free(grid);
}
