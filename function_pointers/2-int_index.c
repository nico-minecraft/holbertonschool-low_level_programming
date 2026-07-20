/**
 * int_index - Searches for an integer
 * Return: Returns index, if invalid, returns -1
 * @array: Array
 * @size: Size of the array
 * @cmp: Pointer to the function to be used to compare values
 */
int int_index(int *array, int size, int (*cmp)(int))
{
int i;

if (array == NULL || cmp == NULL || size <= 0)
return (-1);

for (i = 0; i < size; i++)
{
if (cmp(array[i]) != 0)
return (i);
}

return (-1);
}
