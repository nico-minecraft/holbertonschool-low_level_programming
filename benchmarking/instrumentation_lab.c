#include <stdio.h>
#include <time.h>
#define DATASET_SIZE 50000
#define SEED_VALUE 42u
static int dataset[DATASET_SIZE];
static unsigned int next_value(unsigned int *state)
{
    *state = (*state * 1103515245u) + 12345u;
    return (*state);
}

static void build_dataset(void)
{
    unsigned int state;
    int i;
    state = SEED_VALUE;
    for (i = 0; i < DATASET_SIZE; i++)
        dataset[i] = (int)(next_value(&state) % 100000);
}
static void process_dataset(void)
{
    int i;
    int v;
    for (i = 0; i < DATASET_SIZE; i++)
    {
        v = dataset[i];
        v = (v * 3) + (v / 7) - (v % 11);
        if (v < 0)
            v = -v;
        dataset[i] = v;
    }
}
static unsigned long reduce_checksum(void)
{
    unsigned long sum;
    int i;
    sum = 0;
    for (i = 0; i < DATASET_SIZE; i++)
        sum = (sum * 131ul) + (unsigned long)dataset[i];
    return sum;
}

/* Diff between two timespecs, in milliseconds */
static double timespec_ms(struct timespec start, struct timespec end)
{
    double sec_diff = (double)(end.tv_sec - start.tv_sec);
    double nsec_diff = (double)(end.tv_nsec - start.tv_nsec);
    return sec_diff * 1000.0 + nsec_diff / 1000000.0;
}

int main(void)
{
    unsigned long checksum;
    struct timespec tb1, tb2, tp1, tp2, tch1, tch2;
    double builddur, processdur, checksumdur, totaldur;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tb1);
    build_dataset();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tb2);
    builddur = timespec_ms(tb1, tb2);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp1);
    process_dataset();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp2);
    processdur = timespec_ms(tp1, tp2);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tch1);
    checksum = reduce_checksum();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tch2);
    checksumdur = timespec_ms(tch1, tch2);

    totaldur = builddur + processdur + checksumdur;

    printf("TOTAL seconds: %.6f\n", totaldur);
    printf("BUILD_DATA seconds: %.6f\n", builddur);
    printf("PROCESS seconds: %.6f\n", processdur);
    printf("REDUCE seconds: %.6f\n", checksumdur);

    if (checksum == 0ul)
        printf("impossible\n");

    return 0;
}