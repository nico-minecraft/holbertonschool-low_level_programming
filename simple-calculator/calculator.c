#include <stdio.h>
int main(void)
{

    int n;
    n = 1; /* prime the loop so it runs at least once */

    while (n != 0)
    {

        int n;
        printf("Simple Calculator\n1) Add\n2) Subtract\n3) Multiply\n4) Divide\n0) Quit\n");
        scanf("%d", &n); 
        switch (n) {

        case 1:
        {
            int a, b, c;
            printf("\nAdd\n");
            printf("A: ");
            scanf("%d", &a);
            printf("B: ");
            scanf("%d", &b);
            c = a + b;
            printf("A + B: %d\n\n", c);
            break;
        }
            
        case 2:
        {
            int a, b, c;
            printf("\nSubtract\n");
            printf("A: ");
            scanf("%d", &a);
            printf("B: ");
            scanf("%d", &b);
            c = a - b;
            printf("A - B: %d\n\n", c);
            break;
        }
            break;
        case 3:
        {
            int a, b, c;
            printf("Multiply\n");
            printf("A: ");
            scanf("%d", &a);
            printf("B: ");
            scanf("%d", &b);
            c = a * b;
            printf("A * B: %d\n\n", c);
            break;
        }
        case 4:
            printf("\nDivide\n");
            break;
        case 0:
            printf("\nBye!\n");
            return (0);
        default:
            printf("\nerror\n");
            break;
        }

    }

    return 0;
}