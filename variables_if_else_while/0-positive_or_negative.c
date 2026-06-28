#include <stdlib.h>
#include <time.h>
/**
 * main - Determines whether the random number is positive, negative or zero
 * Return: should always be 0
 */
int main(void)
{
	int n;

	srand(time(0));
	n = rand() - RAND_MAX / 2;
	/* your code goes there */
	if (n > 0)
		printf("%d is positive", n)
	elif(n < 0)
		printf("%d is negative", n)
	elif(n == 0)
		printf("%d is zero", n)
	return (0);
}
