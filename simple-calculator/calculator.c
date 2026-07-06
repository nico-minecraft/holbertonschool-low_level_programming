#include <stdio.h>
/**
 * print_alphabet - Prints the whole alphabet
 * Return: should always be 0
 */
int main(void)
{
    int n;
    printf("Simple Calculator\n1) Add\n2) Subtract\n3) Multiply\n4) Divide\n0) Quit\n");
    scanf("%d", &n); 
    switch (n) {

    case 1:
        printf("Adding uhh");
        break;
    case 2:
        printf("Subtract");
        break;
    case 3:
        printf("Multiply");
        break;
    case 4:
        printf("Divide");
        break;
    case 0:
        printf("Quit");
        break;    
}



    return 0;
}